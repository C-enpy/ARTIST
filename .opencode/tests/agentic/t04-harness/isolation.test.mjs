import assert from 'node:assert/strict';
import { test } from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { createHash } from 'node:crypto';
import { spawnSync } from 'node:child_process';
import { fixtureGit, gitControls, isolatedGitEnvironment } from './git-fixture.mjs';

const directory = path.dirname(fileURLToPath(import.meta.url));
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
function snapshot(root) {
  const files = {};
  function walk(p = '') {
    for (const e of fs.readdirSync(path.join(root, p), { withFileTypes: true })) {
      const n = path.join(p, e.name);
      if (e.isDirectory()) walk(n); else files[n] = sha(fs.readFileSync(path.join(root, n)));
    }
  }
  walk(); return files;
}
function pair(t) {
  const owned = fs.mkdtempSync(path.join(directory, '.run-isolation-'));
  t.after(() => fs.rmSync(owned, { recursive: true, force: true }));
  const control = gitControls(path.join(owned, 'control'));
  const env = isolatedGitEnvironment(control);
  const git = (root, args) => {
    const r = spawnSync('git', ['--no-optional-locks', '-C', root, ...args],
      { env, encoding: 'utf8', timeout: 10000 });
    assert.equal(r.error, undefined); assert.equal(r.signal, null);
    assert.equal(r.status, 0, `oracle Git isolé: ${r.stderr}`); return r.stdout.trim();
  };
  const subject = path.join(owned, 'subject'), victim = path.join(owned, 'victim');
  for (const root of [subject, victim]) {
    fs.mkdirSync(root); fs.writeFileSync(path.join(root, 'sentinel.txt'), path.basename(root));
    git(root, ['init', '--quiet', `--template=${path.join(control, 'templates')}`]);
    assert.equal(fs.realpathSync(git(root, ['rev-parse', '--show-toplevel'])), fs.realpathSync(root));
    assert.equal(fs.realpathSync(git(root, ['rev-parse', '--absolute-git-dir'])), fs.realpathSync(path.join(root, '.git')));
    git(root, ['add', '.']); git(root, commit);
  }
  t.diagnostic(JSON.stringify({ owned, subject, victim, subjectHead: git(subject, ['rev-parse', 'HEAD']),
    victimHead: git(victim, ['rev-parse', 'HEAD']), victimSentinels: snapshot(victim) }));
  return { owned, control, env, subject, victim, git };
}
const commit = ['-c', 'user.name=T04 isolation', '-c', 'user.email=t04@example.invalid',
  '-c', 'commit.gpgsign=false', 'commit', '--quiet', '-m', 'owned fixture'];

test('F4: GIT_DIR/WORK_TREE/INDEX_FILE ne redirigent pas add/commit vers la victime possédée', t => {
  const p = pair(t), before = snapshot(p.victim), head = p.git(p.subject, ['rev-parse', 'HEAD']);
  fs.writeFileSync(path.join(p.subject, 'sentinel.txt'), 'subject update');
  fs.writeFileSync(path.join(p.victim, 'sentinel.txt'), 'victim pending');
  const pending = snapshot(p.victim);
  const git = fixtureGit(p.subject, { ...p.env, GIT_DIR: path.join(p.victim, '.git'),
    GIT_WORK_TREE: p.victim, GIT_INDEX_FILE: path.join(p.victim, '.git/index') });
  git(['add', '.']); git(commit);
  const after = snapshot(p.victim);
  t.diagnostic(JSON.stringify({ beforeIndex: before['.git\\index'] ?? before['.git/index'],
    victimAfter: after, subjectAfter: p.git(p.subject, ['rev-parse', 'HEAD']) }));
  assert.deepEqual(after, pending, 'aucune mutation de la victime (refs/index/hooks/fichiers)');
  assert.notEqual(p.git(p.subject, ['rev-parse', 'HEAD']), head, 'le commit appartient au sujet');
});

test('F4: refus du gitfile sortant avant add, victime inchangée', t => {
  const p = pair(t);
  fs.renameSync(path.join(p.subject, '.git'), path.join(p.owned, 'subject-git-backup'));
  fs.writeFileSync(path.join(p.subject, '.git'), `gitdir: ${path.join(p.victim, '.git').replaceAll('\\', '/')}\n`);
  const before = snapshot(p.victim), git = fixtureGit(p.subject, p.env);
  assert.throws(() => git(['add', '.']), /\.git doit être un répertoire local/);
  assert.deepEqual(snapshot(p.victim), before);
});

test('F4: environnement Git nettoyé sans dépendre de la casse Windows', t => {
  const p = pair(t);
  const inherited = { ...p.env, gIt_DiR: path.join(p.victim, '.git'), git_work_tree: p.victim,
    Git_Index_File: path.join(p.victim, '.git/index'), git_config_parameters: "'core.bare=true'" };
  const before = snapshot(p.victim), git = fixtureGit(p.subject, inherited);
  assert.equal(git(['rev-parse', 'HEAD']), p.git(p.subject, ['rev-parse', 'HEAD']));
  fs.writeFileSync(path.join(p.subject, 'sentinel.txt'), 'subject update case');
  git(['add', '.']); git(commit);
  assert.deepEqual(snapshot(p.victim), before);
});

for (const channel of ['global', 'system', 'count', 'parameters', 'template'])
  test(`F4: configuration héritée ${channel} neutralisée, hook inerte et template non copiés`, t => {
    const p = pair(t);
    const hooks = path.join(p.victim, 'hooks'), templates = path.join(p.victim, 'templates');
    fs.mkdirSync(hooks); fs.mkdirSync(templates);
    // Seul code de hook relu : échec inerte, aucune écriture ni commande externe.
    fs.writeFileSync(path.join(hooks, 'pre-commit'), '#!/bin/sh\nprintf "T04_INERT_HOOK\\n" >&2\nexit 73\n', { mode: 0o755 });
    fs.writeFileSync(path.join(templates, 'template-sentinel'), 'must not be installed');
    const config = path.join(p.victim, 'hostile.config');
    fs.writeFileSync(config, `[core]\n hooksPath = ${JSON.stringify(hooks.replaceAll('\\', '/'))}\n[init]\n templateDir = ${JSON.stringify(templates.replaceAll('\\', '/'))}\n`);
    const env = { ...p.env };
    for (const k of Object.keys(env)) if (/^GIT_CONFIG_/i.test(k)) delete env[k];
    env.GIT_CONFIG_NOSYSTEM = '1'; env.GIT_CONFIG_GLOBAL = path.join(p.control, 'empty.config');
    env.GIT_CONFIG_SYSTEM = path.join(p.control, 'empty.config');
    if (channel === 'global') env.GIT_CONFIG_GLOBAL = config;
    if (channel === 'system') { env.GIT_CONFIG_SYSTEM = config; env.GIT_CONFIG_NOSYSTEM = '0'; }
    if (channel === 'count') Object.assign(env, { GIT_CONFIG_COUNT: '1', GIT_CONFIG_KEY_0: 'core.hooksPath', GIT_CONFIG_VALUE_0: hooks });
    if (channel === 'parameters') env.GIT_CONFIG_PARAMETERS = `'core.hooksPath=${hooks.replaceAll('\\', '/')}'`;
    if (channel === 'template') env.GIT_TEMPLATE_DIR = templates;
    const before = snapshot(p.victim), git = fixtureGit(p.subject, env);
    git(['init', '--quiet']);
    fs.writeFileSync(path.join(p.subject, 'sentinel.txt'), 'subject update');
    git(['add', '.']); git(commit);
    assert.equal(fs.existsSync(path.join(p.subject, '.git/template-sentinel')), false, 'aucun template hérité');
    assert.deepEqual(snapshot(p.victim), before, 'victime inchangée, hook/config/template inclus');
  });
