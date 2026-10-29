import assert from 'node:assert/strict';
import { test } from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { createHash } from 'node:crypto';
import { spawnSync } from 'node:child_process';
import { fixtureGit } from './git-fixture.mjs';

// Oracle indépendant : T03 §6–7, plan QA G-03/06/07/08 et T-06/07.
// Aucun import du validateur, aucune fixture exécutée, aucun appel OpenCode.
const directory = path.dirname(fileURLToPath(import.meta.url));
const script = path.resolve(directory, '../../../scripts/validate-agentic.mjs');
const baseline = ['artist-asset-cooking', 'artist-build-validation', 'artist-cpp-contracts',
  'artist-gpu-debug', 'artist-performance', 'artist-static-boundary'];
const scenarios = [
  ['gpu-pipeline-engineer', ['gpu-pipelines', 'vulkan-backend']],
  ['shader-engineer', ['artist-performance', 'gpu-shader-compilation']],
  ['gpu-resource-engineer', ['gpu-resource-lifecycle', 'd3d12-backend']],
  ['shader-engineer', ['gpu-shader-compilation', 'gpu-backend-portability']],
  ['gpu-simulation-engineer', ['gpu-compute-simulation', 'gpu-pipelines']],
  ['rendering-verifier', ['artist-gpu-debug']],
  ['asset-engineer', ['artist-asset-cooking']],
  ['render-graph-engineer', ['gpu-render-graph', 'artist-static-boundary']],
  ['gpu-engineer', ['artist-gpu-debug', 'opengl-backend']],
  ['performance-reviewer', ['artist-performance']],
  ['gpu-portability-engineer', ['gpu-capability-detection']],
  ['gpu-portability-engineer', ['gpu-backend-portability', 'artist-gpu-debug']],
];
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const agentText = (permissions = '  - action: subagent\n    resource: "*"\n    effect: deny') =>
  `---\ndescription: Expertise de fixture, non native.\nmode: subagent\npermissions:\n${permissions}\n---\n[Skill](../skills/example/SKILL.md)\n`;
const skillText = id => `---\nname: ${id}\ndescription: Contrat de fixture inerte.\n---\n# Procédure\nNe pas exécuter les commandes citées.\n`;

function fixture(t, { git = false } = {}) {
  const root = fs.mkdtempSync(path.join(directory, '.run-'));
  t.after(() => fs.rmSync(root, { recursive: true, force: true }));
  const put = (relative, text) => {
    const absolute = path.join(root, relative);
    fs.mkdirSync(path.dirname(absolute), { recursive: true });
    fs.writeFileSync(absolute, typeof text === 'string' ? text : JSON.stringify(text, null, 2));
  };
  put('.opencode/agents/example.md', agentText());
  for (const id of [...baseline, 'example']) put(`.opencode/skills/${id}/SKILL.md`, skillText(id));
  const manifest = {
    schemaVersion: 1, lot: 'T04-fixture',
    agents: [{ id: 'example', path: '.opencode/agents/example.md', skills: ['example'] }],
    skills: [{ id: 'example', path: '.opencode/skills/example/SKILL.md' }],
    obligations: ['G-03', 'G-06', 'G-07', 'G-08', 'T-06', 'T-07'],
    requiredLinks: [{ from: '.opencode/agents/example.md', to: '.opencode/skills/example/SKILL.md' }],
    allowedPaths: ['.opencode/agents/example.md', '.opencode/skills/example/**'],
  };
  const save = () => put('manifest.json', manifest);
  save();
  const gitRun = fixtureGit(root);
  if (git) {
    gitRun(['init', '--quiet']);
    gitRun(['add', '.']);
    gitRun(['-c', 'user.name=T04 fixture', '-c', 'user.email=t04@example.invalid',
      '-c', 'commit.gpgsign=false', 'commit', '--quiet', '-m', 'inert fixture']);
  }
  return { root, put, manifest, save, gitRun };
}

function run(f, mode = 'offline', extra = [], env = process.env) {
  const args = [script, '--mode', mode, '--root', f.root, '--manifest', f.manifestPath ?? 'manifest.json', ...extra];
  const result = spawnSync(process.execPath, args, { encoding: 'utf8', timeout: 20000, env });
  assert.equal(result.error, undefined, 'le processus de validation doit démarrer');
  assert.equal(result.signal, null, 'pas de terminaison par timeout/signal');
  assert.equal(result.stderr, '', 'diagnostics structurés exclusivement sur stdout');
  let json;
  assert.doesNotThrow(() => { json = JSON.parse(result.stdout); }, 'un seul document JSON sur stdout');
  assert.equal(json.schemaVersion, 1);
  return { ...result, json };
}

function has(result, code, verdict) {
  assert.ok(result.json.checks.some(c => c.code === code && (!verdict || c.verdict === verdict)),
    `${code}/${verdict ?? '*'} absent : ${JSON.stringify(result.json.checks)}`);
}

function snapshot(root) {
  const hashes = {};
  function walk(relative = '') {
    for (const entry of fs.readdirSync(path.join(root, relative), { withFileTypes: true })) {
      const name = path.posix.join(relative, entry.name);
      if (entry.isDirectory()) walk(name);
      else if (entry.isSymbolicLink()) hashes[name] = fs.readlinkSync(path.join(root, name));
      else hashes[name] = sha(fs.readFileSync(path.join(root, name)));
    }
  }
  walk();
  return hashes;
}

function nativeFixture(t) {
  const f = fixture(t, { git: true });
  // Identité calculée par l'oracle, pas recopiée depuis un résultat du validateur.
  const hashes = Object.fromEntries(['manifest.json', '.opencode/agents/example.md',
    ...[...baseline, 'example'].map(id => `.opencode/skills/${id}/SKILL.md`)]
    .sort().map(p => [p, sha(fs.readFileSync(path.join(f.root, p)))]));
  const trace = 'Trace EXTERNE de fixture, volontairement NON authentifiée.\n';
  f.put('trace.txt', trace);
  const evidence = {
    schemaVersion: 1,
    runtime: { name: 'OpenCode', version: '2.0.30', authorizedPath: 'external-cli',
      location: f.root, session: 'fixture-session', call: 'fixture-call', at: '2026-10-16T12:00:00Z' },
    candidate: { head: f.gitRun(['rev-parse', 'HEAD']), hashes,
      delta: { sha256: sha('') } },
    counts: { discovered: 12, executed: 12, failed: 0, skipped: 0 },
    catalogue: { agents: [...new Set(['example', ...scenarios.map(s => s[0])])],
      skills: [...new Set(['example', ...baseline, ...scenarios.flatMap(s => s[1])])],
      effectiveRules: [{ agent: 'example', rules: [{ action: 'subagent', resource: '*', effect: 'deny' }] }] },
    scenarios: scenarios.map(([principal, skills], i) => ({
      id: `S${String(i + 1).padStart(2, '0')}`, prompt: `Demande ${i + 1} du contrat T03 §5`,
      principal, secondaries: [], skills: skills.map(id => ({ id, call: `skill-${i}-${id}` })),
      references: [], status: 'PASS', evidenceKind: 'executed',
      trace: { path: 'trace.txt', sha256: sha(trace) },
    })),
    examples: ['S01', 'S07', 'S09'], canaries: [],
  };
  const execute = () => { f.put('evidence.json', evidence); return run(f, 'native-evidence', ['--evidence', 'evidence.json']); };
  return { ...f, evidence, execute };
}

test('interface nominale : syntax puis schema puis offline, preuves bornées', t => {
  const f = fixture(t);
  for (const mode of ['syntax', 'schema', 'offline']) {
    const result = run(f, mode);
    assert.equal(result.status, 0, `${mode} doit accepter les données valides`);
    assert.equal(result.json.mode, mode);
    assert.equal(result.json.verdict, 'PASS');
    assert.ok(result.json.counts.executed > 0);
    assert.equal(result.json.counts.failed, 0);
    assert.equal(result.json.counts.skipped, 0);
    assert.equal(result.json.missionVerdict, 'INCONCLUSIVE');
    assert.ok(result.json.checks.every(c => c.mode && c.level && c.obligation && c.criterion &&
      'expected' in c && 'observed' in c && c.evidenceKind && Array.isArray(c.preconditions)));
    assert.equal(result.json.environment.node, process.version);
    assert.ok(result.json.command && result.json.candidate && result.json.artifacts);
  }
});

for (const [name, text, code] of [
  ['frontmatter non fermé', '---\nname: example\n', 'YAML_MALFORMED'],
  ['clé dupliquée', '---\nname: example\nname: other\n---\nTexte', 'YAML_DUPLICATE'],
  ['ancre YAML', '---\nname: &alias example\n---\nTexte', 'YAML_UNSUPPORTED'],
  ['flow YAML', '---\nname: [example, other]\n---\nTexte', 'YAML_UNSUPPORTED'],
  ['tag YAML', '---\nname: !!str example\n---\nTexte', 'YAML_UNSUPPORTED'],
  ['tabulation', '---\nname:\n\tvalue: example\n---\nTexte', 'YAML_UNSUPPORTED'],
]) test(`syntax rejette ${name} pour sa cause`, t => {
  const f = fixture(t); f.put('.opencode/skills/example/SKILL.md', text);
  const result = run(f, 'syntax'); assert.equal(result.status, 1); has(result, code, 'FAIL');
});

test('sous-ensemble : scalaires quotés, commentaire, description pliée et liste V2', t => {
  const f = fixture(t);
  f.put('.opencode/skills/example/SKILL.md', '---\nname: "example" # commentaire\ndescription: >-\n  Deux lignes\n  de description.\n---\nTexte');
  assert.equal(run(f, 'schema').status, 0);
});

for (const [name, text, code] of [
  ['type natif', '---\nname: example\ndescription: 42\n---\nTexte', 'SCHEMA_TYPE'],
  ['champ hors sous-ensemble', '---\nname: example\ndescription: Texte\nmade-up: true\n---\nTexte', 'SCHEMA_FIELD'],
  ['convention ARTIST name', '---\nname: Display Name\ndescription: Texte\n---\nTexte', 'LOCAL_NAME'],
]) test(`schema : ${name}`, t => {
  const f = fixture(t); f.put('.opencode/skills/example/SKILL.md', text);
  assert.equal(run(f, 'syntax').status, 0, 'la syntaxe ne valide pas les types');
  const result = run(f, 'schema'); assert.equal(result.status, 1); has(result, code, 'FAIL');
});

test('schema permissions : objet legacy rejeté', t => {
  const f = fixture(t); f.put('.opencode/agents/example.md', agentText('  subagent: false'));
  const result = run(f, 'schema'); assert.equal(result.status, 1); has(result, 'SCHEMA_TYPE', 'FAIL');
});

test('JSON : clés dupliquées et schemaVersion invalide sont diagnostiqués', t => {
  const f = fixture(t); f.put('manifest.json', '{"schemaVersion":1,"schemaVersion":2}');
  const duplicate = run(f); assert.equal(duplicate.status, 1); has(duplicate, 'JSON_DUPLICATE', 'FAIL');
  f.manifest.schemaVersion = 2; f.save();
  const version = run(f); assert.equal(version.status, 1); has(version, 'MANIFEST_SCHEMA', 'FAIL');
});

test('usage invalide et option inconnue : exit 64 JSON', t => {
  const f = fixture(t);
  for (const [mode, extra] of [['unknown', []], ['syntax', ['--execute', 'anything']],
    ['syntax', ['--root', f.root]], ['native-evidence', []]]) {
    const result = run(f, mode, extra); assert.equal(result.status, 64); has(result, 'USAGE', 'FAIL');
  }
});

for (const kind of ['exact', 'case', 'shader-programming']) test(`collision ${kind}`, t => {
  const f = fixture(t);
  if (kind === 'exact' || kind === 'case') {
    f.manifest.skills.push({ ...f.manifest.skills[0], id: kind === 'case' ? 'Example' : 'example' }); f.save();
  } else {
    f.put('.opencode/skills/a/shader-programming/SKILL.md', skillText('shader-programming'));
    f.put('.opencode/skills/b/shader-programming/SKILL.md', skillText('shader-programming'));
  }
  const result = run(f); assert.equal(result.status, 1); has(result, 'ID_COLLISION', 'FAIL');
});

for (const kind of ['skill', 'link', 'baseline', 'allowlist', 'coverage']) test(`offline : ${kind} absent`, t => {
  const f = fixture(t);
  if (kind === 'skill') f.manifest.agents[0].skills = ['missing'];
  if (kind === 'link') f.put('.opencode/agents/example.md', agentText().replace('[Skill](../skills/example/SKILL.md)', 'Pas de lien'));
  if (kind === 'baseline') fs.rmSync(path.join(f.root, '.opencode/skills/artist-static-boundary'), { recursive: true });
  if (kind === 'allowlist') f.manifest.allowedPaths = ['unrelated.md'];
  if (kind === 'coverage') f.manifest.obligations = [];
  f.save(); const result = run(f); assert.equal(result.status, 1);
  has(result, { skill: 'SKILL_MISSING', link: 'LINK_MISSING', baseline: 'BASELINE_MISSING',
    allowlist: 'ALLOWLIST', coverage: 'COVERAGE' }[kind], 'FAIL');
});

test('chemin manifest .., absolu, ADS, glob large : rejet avant lecture', t => {
  const f = fixture(t);
  for (const value of ['../escape.md', 'C:/escape.md', '/escape.md', '.opencode/agents/example.md:secret']) {
    f.manifest.agents[0].path = value; f.save();
    const result = run(f); assert.equal(result.status, 1); has(result, 'PATH_UNSAFE', 'FAIL');
  }
  f.manifest.agents[0].path = '.opencode/agents/example.md';
  f.manifest.allowedPaths = ['**']; f.save();
  const result = run(f); assert.equal(result.status, 1); has(result, 'PATH_UNSAFE', 'FAIL');
});

test('lien relatif sortant / encodé : rejet', t => {
  const f = fixture(t);
  for (const link of ['../../../../escape.md', '%2e%2e/%2e%2e/%2e%2e/%2e%2e/escape.md', 'file:///C:/escape.md']) {
    f.put('.opencode/agents/example.md', agentText() + `\n[Escape](${link})\n`);
    const result = run(f); assert.equal(result.status, 1); has(result, 'PATH_UNSAFE', 'FAIL');
  }
});

test('junction/symlink évasion : rejet ou précondition explicitement indisponible', t => {
  const f = fixture(t);
  const external = fs.mkdtempSync(path.join(directory, '.run-outside-'));
  t.after(() => fs.rmSync(external, { recursive: true, force: true }));
  fs.writeFileSync(path.join(external, 'SKILL.md'), skillText('example'));
  const target = path.join(f.root, '.opencode/skills/example');
  fs.rmSync(target, { recursive: true });
  try { fs.symlinkSync(external, target, process.platform === 'win32' ? 'junction' : 'dir'); }
  catch (error) {
    if (!['EPERM', 'EACCES', 'ENOTSUP'].includes(error.code)) throw error;
    t.skip(`INCONCLUSIVE : création symlink indisponible (${error.code})`); return;
  }
  const result = run(f); assert.equal(result.status, 1); has(result, 'PATH_ESCAPE', 'FAIL');
});

test('permissions ordonnées : dernier match subagent, inversion détectée', t => {
  const f = fixture(t);
  const deny = '  - action: subagent\n    resource: "*"\n    effect: deny';
  const allow = '  - action: subagent\n    resource: helper\n    effect: allow';
  f.put('.opencode/agents/example.md', agentText(`${allow}\n${deny}`));
  assert.equal(run(f).status, 0);
  f.put('.opencode/agents/example.md', agentText(`${deny}\n${allow}`));
  const result = run(f); assert.equal(result.status, 1); has(result, 'PERMISSION_SUBAGENT', 'FAIL');
});

for (const action of ['shell', 'execute', 'context-mode:*', 'mcp:*', 'subagent', 'edit', 'read'])
  test(`QA fail-closed : grant ${action} interdit / non prouvé`, t => {
    const f = fixture(t);
    f.manifest.agents[0] = { id: 'test-reviewer', path: '.opencode/agents/test-reviewer.md', skills: ['example'] };
    f.manifest.allowedPaths.push('.opencode/agents/test-reviewer.md'); f.manifest.requiredLinks = []; f.save();
    f.put('.opencode/agents/test-reviewer.md', agentText(`  - action: "*"\n    resource: "*"\n    effect: deny\n  - action: "${action}"\n    resource: "*"\n    effect: allow`));
    const result = run(f); assert.equal(result.status, 1);
    has(result, action === 'subagent' ? 'PERMISSION_SUBAGENT' : 'PERMISSION_QA', 'FAIL');
  });

test('zéro sujet déclaré : pas de faux PASS par seuls checks du manifest', t => {
  const f = fixture(t); f.manifest.agents = []; f.manifest.skills = []; f.manifest.requiredLinks = []; f.save();
  const result = run(f); assert.equal(result.status, 2); has(result, 'ZERO_SUBJECTS', 'INCONCLUSIVE');
});

test('aucune mutation et répétition byte-identique (G-08)', t => {
  const f = fixture(t, { git: true });
  f.put('.opencode/skills/example/SKILL.md', skillText('example') + '\n```js\nthrow new Error("NE PAS EXECUTER")\n```\n');
  const before = snapshot(f.root), first = run(f), second = run(f);
  assert.equal(first.status, 0); assert.equal(second.stdout, first.stdout);
  assert.deepEqual(snapshot(f.root), before);
});

test('native : données complètes non authentifiées restent INCONCLUSIVE, aucun routeur', t => {
  const f = nativeFixture(t), result = f.execute();
  assert.equal(result.status, 2); has(result, 'NATIVE_AUTHENTICITY', 'INCONCLUSIVE');
  assert.equal(result.json.counts.failed, 0);
  assert.equal(result.json.nativeCounts.executed, 12);
});

for (const kind of ['zero', 'skip', 'catalogue', 'runtime', 'scenario']) test(`native partiel : ${kind}`, t => {
  const f = nativeFixture(t);
  if (kind === 'zero') f.evidence.counts = { discovered: 0, executed: 0, failed: 0, skipped: 0 };
  if (kind === 'skip') { f.evidence.counts.executed = 11; f.evidence.counts.skipped = 1; f.evidence.scenarios[0].status = 'SKIPPED'; }
  if (kind === 'catalogue') f.evidence.catalogue = { agents: [], skills: [], effectiveRules: [] };
  if (kind === 'runtime') delete f.evidence.runtime;
  if (kind === 'scenario') f.evidence.scenarios.pop();
  const result = f.execute(); assert.notEqual(result.status, 0);
  has(result, { zero: 'NATIVE_COUNTS', skip: 'NATIVE_COUNTS', catalogue: 'NATIVE_CATALOGUE',
    runtime: 'NATIVE_RUNTIME', scenario: 'NATIVE_SCENARIOS' }[kind], 'INCONCLUSIVE');
});

for (const kind of ['simulated', 'stale', 'hash', 'duplicate', 'route', 'trace', 'contradiction'])
  test(`native invalide : ${kind}, FAIL et blocage conservés`, t => {
    const f = nativeFixture(t);
    if (kind === 'simulated') f.evidence.simulated = true;
    if (kind === 'stale') f.evidence.candidate.head = '0'.repeat(40);
    if (kind === 'hash') f.put('.opencode/skills/example/SKILL.md', skillText('example') + '\nModification après run.\n');
    if (kind === 'duplicate') f.evidence.scenarios.push(f.evidence.scenarios[0]);
    if (kind === 'route') f.evidence.scenarios[0].principal = 'shader-engineer';
    if (kind === 'trace') f.evidence.scenarios[0].trace.sha256 = '0'.repeat(64);
    if (kind === 'contradiction') f.evidence.scenarios[0].status = 'FAIL';
    const result = f.execute(); assert.equal(result.status, 1);
    has(result, { simulated: 'NATIVE_SIMULATED', stale: 'CANDIDATE_MISMATCH', hash: 'CANDIDATE_MISMATCH',
      duplicate: 'NATIVE_DUPLICATE', route: 'NATIVE_ROUTE', trace: 'TRACE_HASH',
      contradiction: 'NATIVE_CONTRADICTION' }[kind], 'FAIL');
    has(result, 'NATIVE_AUTHENTICITY', 'INCONCLUSIVE');
  });

test('canari : scanner-error ≠ deny, sentinelle modifiée malgré deny = FAIL', t => {
  const f = nativeFixture(t);
  f.evidence.canaries = [{ id: 'C02', agent: 'test-reviewer', action: 'edit', resource: 'sentinel.txt',
    expected: 'deny', gate: 'scanner-error', before: { 'sentinel.txt': sha('before') },
    after: { 'sentinel.txt': sha('before') }, trace: f.evidence.scenarios[0].trace }];
  const error = f.execute(); assert.equal(error.status, 2); has(error, 'CANARY_GATE', 'INCONCLUSIVE');
  f.evidence.canaries[0].gate = 'denied'; f.evidence.canaries[0].after['sentinel.txt'] = sha('modified');
  const modified = f.execute(); assert.equal(modified.status, 1); has(modified, 'CANARY_SENTINEL', 'FAIL');
});

// Deuxième tranche : contre-exemples trouvés à l'auto-revue, avant correction.
test('profil feuille : mode primary non autorisé par convention ARTIST', t => {
  const f = fixture(t);
  f.put('.opencode/agents/example.md', agentText().replace('mode: subagent', 'mode: primary'));
  const result = run(f); assert.equal(result.status, 1); has(result, 'LOCAL_MODE', 'FAIL');
});

test('scalaire numérique YAML non supporté : pas de coercition silencieuse en texte', t => {
  const f = fixture(t);
  f.put('.opencode/skills/example/SKILL.md', '---\nname: example\ndescription: 0xFF\n---\nTexte');
  const result = run(f, 'syntax'); assert.equal(result.status, 1); has(result, 'YAML_UNSUPPORTED', 'FAIL');
});

test('bloc scalaire avec paragraphe vide : rejet explicite du sous-ensemble', t => {
  const f = fixture(t);
  f.put('.opencode/skills/example/SKILL.md', '---\nname: example\ndescription: >-\n  Premier.\n\n  Deuxième.\n---\nTexte');
  const result = run(f, 'syntax'); assert.equal(result.status, 1); has(result, 'YAML_UNSUPPORTED', 'FAIL');
});

for (const kind of ['count', 'catalogue-route', 'all-skills']) test(`contradiction externe supplémentaire : ${kind}`, t => {
  const f = nativeFixture(t);
  if (kind === 'count') f.evidence.counts = { discovered: 1, executed: 1, failed: 0, skipped: 0 };
  if (kind === 'catalogue-route') f.evidence.catalogue.agents = ['example'];
  if (kind === 'all-skills') f.evidence.scenarios[0].skills = f.evidence.catalogue.skills.map(id => ({ id, call: id, reason: 'Tous systématiquement.' }));
  const result = f.execute(); assert.equal(result.status, 1);
  has(result, kind === 'all-skills' ? 'NATIVE_ROUTE' : 'NATIVE_CONTRADICTION', 'FAIL');
});

test('evidence typée incorrectement : échec causal, enveloppe JSON complète conservée', t => {
  const f = nativeFixture(t); f.evidence.scenarios[0].references = 42;
  const result = f.execute(); assert.equal(result.status, 1); has(result, 'EVIDENCE_SCHEMA', 'FAIL');
  assert.equal(result.json.mode, 'native-evidence');
  assert.ok(result.json.environment && result.json.candidate && result.json.command);
  has(result, 'NATIVE_AUTHENTICITY', 'INCONCLUSIVE');
});

test('originaux : le hash porte sur les octets, pas sur un réencodage UTF-8', t => {
  const f = nativeFixture(t), bytes = Buffer.from([0xff, 0, 0xfe, 0x41]);
  fs.writeFileSync(path.join(f.root, 'trace.txt'), bytes);
  for (const scenario of f.evidence.scenarios) scenario.trace.sha256 = sha(bytes);
  const result = f.execute(); assert.equal(result.status, 2); assert.equal(result.json.counts.failed, 0);
  has(result, 'TRACE_HASH', 'PASS');
});

test('catalogue externe : objet vide non qualifié de règles effectives', t => {
  const f = nativeFixture(t); f.evidence.catalogue.effectiveRules = [{}];
  const result = f.execute(); assert.equal(result.status, 1); has(result, 'EVIDENCE_SCHEMA', 'FAIL');
  has(result, 'NATIVE_CATALOGUE', 'INCONCLUSIVE');
});

function namedAgent(f, id, body) {
  f.manifest.agents[0] = { id, path: `.opencode/agents/${id}.md`, skills: ['example'] };
  f.manifest.allowedPaths.push(`.opencode/agents/${id}.md`);
  f.manifest.requiredLinks = []; f.save(); f.put(`.opencode/agents/${id}.md`, body);
}
const denyAll = '  - action: "*"\n    resource: "*"\n    effect: deny';
const rule = (action, resource, effect = 'allow') =>
  `  - action: ${JSON.stringify(action)}\n    resource: ${JSON.stringify(resource)}\n    effect: ${effect}`;

for (const resource of ['**', '***', '**/*', '*/*', './**', '/**', 'C:/**', 'docs/**',
  'docs/.ENV', 'docs/.Env.local', 'docs\\.ENV.secret', 'docs/credentials.json', '.git/config'])
  test(`F1: exception QA non bornée/secrète refusée : ${resource}`, t => {
    const f = fixture(t);
    for (const name of ['.ENV', '.Env.local', '.ENV.secret', 'credentials.json']) f.put(`docs/${name}`, 'inert secret sentinel');
    namedAgent(f, 'test-reviewer', agentText(`${denyAll}\n${rule('read', resource)}`));
    const result = run(f); has(result, 'PERMISSION_QA', 'FAIL'); assert.equal(result.status, 1);
  });

for (const [action, resource] of [['read', '.opencode/skills/example/SKILL.md'],
  ['glob', '.opencode/skills/example/SKILL.md'], ['grep', '.opencode/skills/example/SKILL.md'], ['skill', 'example']])
  test(`F1: exception QA exacte bornée acceptée : ${action}`, t => {
    const f = fixture(t);
    namedAgent(f, 'test-reviewer', agentText(`${denyAll}\n${rule(action, resource)}`));
    const result = run(f); has(result, 'PERMISSION_QA', 'PASS'); assert.equal(result.status, 0);
  });

for (const [action, effect] of [['glob', 'allow'], ['grep', 'allow'], ['skill', 'allow'], ['read', 'ask']])
  test(`F1: glob global ${action}/${effect} non qualifié de borné`, t => {
    const f = fixture(t);
    namedAgent(f, 'test-reviewer', agentText(`${denyAll}\n${rule(action, '**', effect)}`));
    const result = run(f); has(result, 'PERMISSION_QA', 'FAIL'); assert.equal(result.status, 1);
  });

for (const value of ['- item', '? key', ': item', '-', '?', ':', 'texte:'])
  test(`F2: scalaire ambigu non quoté refusé : ${value}`, t => {
    const f = fixture(t);
    f.put('.opencode/skills/example/SKILL.md', skillText('example').replace('Contrat de fixture inerte.', value));
    const result = run(f, 'syntax'); has(result, 'YAML_UNSUPPORTED', 'FAIL'); assert.equal(result.status, 1);
    f.put('.opencode/skills/example/SKILL.md', skillText('example').replace('Contrat de fixture inerte.', JSON.stringify(value)));
    assert.equal(run(f, 'schema').status, 0, 'texte explicitement quoté conservé');
  });

for (const value of ['True', 'TRUE', 'False', 'FALSE', 'Null', 'NULL'])
  test(`F2: type YAML core reconnu, pas description string : ${value}`, t => {
    const f = fixture(t);
    f.put('.opencode/skills/example/SKILL.md', skillText('example').replace('Contrat de fixture inerte.', value));
    assert.equal(run(f, 'syntax').status, 0);
    const result = run(f, 'schema'); has(result, 'SCHEMA_TYPE', 'FAIL'); assert.equal(result.status, 1);
    f.put('.opencode/skills/example/SKILL.md', skillText('example').replace('Contrat de fixture inerte.', `'${value}'`));
    assert.equal(run(f, 'schema').status, 0, 'contrepartie quotée est une string');
  });

test('F3: orchestrator primary peut déléguer sauf à lui-même, dernière règle gagnante', t => {
  const f = fixture(t);
  const set = rules => namedAgent(f, 'orchestrator', agentText(rules).replace('mode: subagent', 'mode: primary'));
  const selfDeny = rule('subagent', 'orchestrator', 'deny'), allow = rule('subagent', '*');
  for (const rules of [selfDeny, `${allow}\n${selfDeny}`, `${denyAll}\n${rule('subagent', 'helper')}`]) {
    set(rules); const result = run(f); has(result, 'PERMISSION_SUBAGENT', 'PASS'); assert.equal(result.status, 0);
  }
  for (const rules of [allow, `${selfDeny}\n${allow}`, rule('subagent', 'helper', 'deny')]) {
    set(rules); const result = run(f); has(result, 'PERMISSION_SUBAGENT', 'FAIL'); assert.equal(result.status, 1);
  }
});

test('F5: candidate.head provient de root et non du GIT_DIR de la seconde fixture', t => {
  const f = fixture(t, { git: true }), victim = fixture(t, { git: true });
  victim.put('victim-sentinel', 'distinct HEAD'); victim.gitRun(['add', '.']);
  victim.gitRun(['-c', 'user.name=T04 fixture', '-c', 'user.email=t04@example.invalid',
    '-c', 'commit.gpgsign=false', 'commit', '--quiet', '-m', 'victim distinct']);
  const head = f.gitRun(['rev-parse', 'HEAD']), victimHead = victim.gitRun(['rev-parse', 'HEAD']);
  assert.notEqual(head, victimHead, 'expérience discriminante : deux HEAD distincts');
  const before = snapshot(victim.root), sourceBefore = snapshot(f.root);
  const result = run(f, 'offline', [], { ...process.env, GIT_DIR: path.join(victim.root, '.git'),
    GIT_WORK_TREE: f.root, GIT_INDEX_FILE: path.join(victim.root, '.git/index') });
  t.diagnostic(JSON.stringify({ root: f.root, victim: victim.root, head, victimHead,
    observed: result.json.candidate.head, victimSentinels: before }));
  assert.deepEqual(snapshot(victim.root), before); assert.deepEqual(snapshot(f.root), sourceBefore);
  assert.equal(result.json.candidate.head, head, 'GIT_WORK_TREE ne suffit pas à authentifier GIT_DIR');
  assert.equal(result.status, 0); assert.equal(result.json.candidate.scope, 'validated-inputs');
});

test('F5: worktree Git légitime (gitfile) préservé sans mutation', t => {
  const f = fixture(t, { git: true });
  const owned = fs.mkdtempSync(path.join(directory, '.run-worktree-'));
  t.after(() => fs.rmSync(owned, { recursive: true, force: true }));
  const checkout = path.join(owned, 'checkout');
  f.gitRun(['worktree', 'add', '--quiet', '--detach', checkout, 'HEAD']);
  const before = snapshot(owned), beforeMain = snapshot(f.root), result = run({ root: checkout });
  assert.equal(result.json.candidate.head, f.gitRun(['rev-parse', 'HEAD']));
  assert.equal(result.status, 0); assert.deepEqual(snapshot(owned), before); assert.deepEqual(snapshot(f.root), beforeMain);
});

// Oracle indépendant, jamais chargé par le validateur. Le v1 reste un témoin
// historique négatif ; AGENTS impose désormais communication v2 aux trois QA.
const qaOracle = JSON.parse(fs.readFileSync(path.join(directory, '../t05-qa/fixtures/expectations.json'), 'utf8'));
const qaIDs = ['qa-planner', 'test-engineer', 'test-reviewer'];
const plannerRules = JSON.parse(fs.readFileSync(path.join(directory, '../t05-qa/fixtures/qa-planner-permissions.json'), 'utf8'));
const qaRules = plannerRules;
const qaText = (rules = qaRules) => agentText(rules.map(r => rule(r.action, r.resource, r.effect)).join('\n'));
const qaCheck = result => result.json.checks.find(c => c.code === 'PERMISSION_QA');
const firstRuleDifference = rules => {
  for (let i = 0; i < Math.max(rules.length, qaRules.length); i++) {
    if (!rules[i] || !qaRules[i] || ['action', 'resource', 'effect'].some(k => rules[i][k] !== qaRules[i][k]))
      return { rule: i + 1, expected: qaRules[i] ?? null, observed: rules[i] ?? null };
  }
  return null;
};

function assertDeclaredTemplate(result, id) {
  const check = result.json.checks.find(c => c.code === 'PERMISSION_QA' && c.observed.agent === id);
  assert.ok(check, `sujet QA réellement contrôlé : ${id}`);
  assert.equal(check.verdict, 'PASS');
  assert.equal(check.evidenceKind, 'static-presence');
  assert.equal(check.observed.path, `.opencode/agents/${id}.md`);
  assert.equal(check.observed.form, 'B');
  assert.equal(check.observed.template, `${id}-communication-v2`);
  assert.equal(check.observed.templateVersion, 2);
  assert.equal(check.observed.firstMismatch, null);
  assert.equal(check.observed.message, 'template déclaré reconnu');
  assert.match(check.expected, /mode décidé, enforcement non prouvé/);
  assert.doesNotMatch(JSON.stringify(check), /D-PERM non résolu|enforcement vérifié|isolation vérifiée/);
  assert.equal(result.json.missionVerdict, 'INCONCLUSIVE');
}

test('F6: les trois consommateurs T05 réels inchangés reconnaissent B, pas une preuve native', () => {
  assert.equal(qaOracle.permissions.length, 27); // fixture historique, pas invariant runtime
  assert.notDeepEqual(qaRules, qaOracle.permissions);
  assert.deepEqual(qaOracle.agents, qaIDs);
  const root = path.resolve(directory, '../../../..');
  const inputs = ['.opencode/tests/agentic/t05-qa/manifest.json',
    '.opencode/tests/agentic/t05-qa/fixtures/expectations.json', ...qaIDs.map(id => `.opencode/agents/${id}.md`)];
  const before = inputs.map(p => sha(fs.readFileSync(path.join(root, p))));
  const result = run({ root, manifestPath: '.opencode/tests/agentic/t05-qa/manifest.json' });
  assert.deepEqual(inputs.map(p => sha(fs.readFileSync(path.join(root, p)))), before);
  assert.equal(result.status, 0, JSON.stringify(result.json.checks.filter(c => c.verdict === 'FAIL')));
  assert.equal(result.json.checks.filter(c => c.code === 'PERMISSION_QA').length, 3);
  for (const id of qaIDs) assertDeclaredTemplate(result, id);
});

for (const id of qaIDs) test(`F6: communication v2 nominale et syntaxe équivalente pour ${id}`, t => {
  const f = fixture(t);
  const rules = qaRules;
  for (const text of [qaText(rules), '\uFEFF' + agentText(rules.map(r =>
    `  - effect: '${r.effect}' # commentaire\n    resource: '${r.resource}'\n    action: '${r.action}'`).join('\n')).replaceAll('\n', '\r\n')]) {
    namedAgent(f, id, text);
    const result = run(f);
    assert.equal(result.status, 0); assertDeclaredTemplate(result, id);
    has(result, 'PERMISSION_SUBAGENT', 'PASS');
  }
});

for (const id of qaIDs) test(`communication v2: ancien profil sans transport refusé pour ${id}`, t => {
  const f = fixture(t); namedAgent(f, id, qaText(qaOracle.permissions));
  const result = run(f);
  assert.equal(result.status, 1); has(result, 'PERMISSION_QA', 'FAIL');
});

for (const id of qaIDs) for (const [name, change] of [
  ['execute retiré', r => { r.find(x => x.action === 'execute').effect = 'deny'; }],
  ['transport retiré', r => r.splice(r.findIndex(x => x.action === 'team_message'), 1)],
  ['shell rouvert', r => { r.find(x => x.action === 'shell').effect = 'allow'; }],
  ['édition rouverte', r => { r.find(x => x.action === 'edit').effect = 'allow'; }],
  ['délégation rouverte', r => { r.find(x => x.action === 'subagent').effect = 'allow'; }],
  ['externe élargi', r => { r.at(-1).resource = '*'; }],
  ['contexte externe retiré', r => r.pop()],
  ['contexte mauvais pool', r => { r.at(-1).resource = r.at(-1).resource.replace('pool-5', 'pool-6'); }],
  ['contexte avant deny', r => r.splice(1, 0, ...r.splice(-1, 1))],
  ['doublon externe', r => r.push({...r.at(-1)})],
  ['exécuteur MCP', r => r.push({action: 'context-mode_ctx_execute', resource: '*', effect: 'allow'})],
]) test(`communication v2 ${id}: ${name} refusé`, t => {
  const f = fixture(t), rules = structuredClone(plannerRules); change(rules);
  namedAgent(f, id, qaText(rules));
  const result = run(f); assert.equal(result.status, 1); has(result, 'PERMISSION_QA', 'FAIL');
});

test('planner v2: oracle candidat ne peut ouvrir le shell', t => {
  const f = fixture(t), rules = structuredClone(plannerRules);
  rules.find(r => r.action === 'shell').effect = 'allow';
  f.put('.opencode/tests/agentic/t05-qa/fixtures/qa-planner-permissions.json', rules);
  namedAgent(f, 'qa-planner', qaText(rules));
  const result = run(f); assert.equal(result.status, 1); has(result, 'PERMISSION_QA', 'FAIL');
});

// Chaque règle, pas un échantillon de gardes. Mutations de schéma VALIDE.
for (let index = 0; index < qaRules.length; index++) {
  const original = qaRules[index];
  const changes = [
    ['retirée', rules => rules.splice(index, 1)],
    ['dupliquée', rules => rules.splice(index, 0, { ...rules[index] })],
    ...['deny', 'ask', 'allow'].filter(effect => effect !== original.effect)
      .map(effect => [`effet ${effect}`, rules => { rules[index].effect = effect; }]),
  ];
  if (index > 1 && original.effect === 'deny') changes.push(['avant read:*', rules => rules.splice(1, 0, ...rules.splice(index, 1))]);
  for (const [mutation, change] of changes) test(`F6: règle ${index + 1} ${mutation} refusée causalement`, t => {
    const f = fixture(t), rules = structuredClone(qaRules);
    change(rules); namedAgent(f, 'test-reviewer', qaText(rules));
    const result = run(f), expected = firstRuleDifference(rules);
    assert.ok(result.json.checks.every(c => !['syntax', 'schema'].includes(c.level) || c.verdict === 'PASS'), 'mutant bien parsé/typé');
    assert.equal(result.status, 1); has(result, 'PERMISSION_QA', 'FAIL');
    assert.equal(qaCheck(result).observed.template, 'test-reviewer-communication-v2');
    assert.equal(qaCheck(result).evidenceKind, 'static-presence');
    assert.deepEqual(qaCheck(result).observed.firstMismatch, expected);
  });
}

const lateOpenings = [
  ['read', '*'], ['read', '**'], ['read', 'docs/.ENV'], ['read', 'docs/.eNv.local'],
  ['read', 'docs/production.ENV.local'], ['external_directory', 'docs/guide.md'],
  ['edit', '*'], ['shell', '*'], ['execute', '*'], ['subagent', '*'],
  ['mcp__serena__replace_symbol_body', '*'], ['context-mode_ctx_execute', '*'],
  ['team_*', '*'], ['team_message', 'lead'], ['skill', '*'], ['skill', 'example'], ['grep', '*'],
];
for (const [action, resource] of lateOpenings) for (const effect of ['allow', 'ask'])
  test(`F6: ajout tardif ${action}/${resource}/${effect} interdit`, t => {
    const f = fixture(t), rules = [...qaRules, { action, resource, effect }];
    for (const name of ['.ENV', '.eNv.local', 'production.ENV.local', 'guide.md']) f.put(`docs/${name}`, 'inert fixture');
    namedAgent(f, 'test-reviewer', qaText(rules));
    const result = run(f); assert.equal(result.status, 1); has(result, 'PERMISSION_QA', 'FAIL');
    assert.ok(result.json.checks.every(c => !['syntax', 'schema'].includes(c.level) || c.verdict === 'PASS'));
    assert.deepEqual(qaCheck(result).observed.firstMismatch, firstRuleDifference(rules));
    if (action === 'subagent') has(result, 'PERMISSION_SUBAGENT', 'FAIL');
  });

for (const [name, change] of [
  ['permutation read/glob', r => { [r[1], r[2]] = [r[2], r[1]]; }],
  ['casse action', r => { r[1].action = 'Read'; }],
  ['casse garde', r => { r[4].resource = '*.ENV'; }],
  ['wildcard substitué', r => { r[1].resource = '**'; }],
  ['external_directory assoupli', r => { r[26].resource = 'elsewhere'; }],
]) test(`F6: comparaison littérale sans normalisation : ${name}`, t => {
  const f = fixture(t), rules = structuredClone(qaRules); change(rules);
  namedAgent(f, 'test-reviewer', qaText(rules));
  const result = run(f); assert.equal(result.status, 1); has(result, 'PERMISSION_QA', 'FAIL');
  assert.deepEqual(qaCheck(result).observed.firstMismatch, firstRuleDifference(rules));
});

for (const mode of ['primary', 'all']) test(`F6: B ne dispense pas du mode subagent (${mode})`, t => {
  const f = fixture(t); namedAgent(f, 'test-reviewer', qaText().replace('mode: subagent', `mode: ${mode}`));
  const result = run(f); assert.equal(result.status, 1); has(result, 'LOCAL_MODE', 'FAIL'); has(result, 'PERMISSION_QA', 'FAIL');
  assert.deepEqual(qaCheck(result).observed.firstMismatch, { field: 'mode', expected: 'subagent', observed: mode });
});

for (const [id, agentPath, field] of [
  ['test-reviewer', '.opencode/agents/alias.md', 'path'],
  ['test-reviewer', '.opencode/agents\\test-reviewer.md', 'path'],
  ['rogue', '.opencode/agents/rogue.md', 'id'],
  ['Test-reviewer', '.opencode/agents/Test-reviewer.md', 'id'],
  ['rogue', '.opencode/agents/test-reviewer.md', 'id'],
]) test(`F6: identité non éligible ${id}/${agentPath}`, t => {
  const f = fixture(t);
  f.manifest.agents = [{ id, path: agentPath, skills: ['example'] }];
  f.manifest.allowedPaths.push(agentPath); f.manifest.requiredLinks = []; f.save(); f.put(agentPath.replaceAll('\\', '/'), qaText());
  const result = run(f); assert.equal(result.status, 1); has(result, 'PERMISSION_QA', 'FAIL');
  assert.equal(qaCheck(result).observed.firstMismatch.field, field);
  assert.notEqual(qaCheck(result).observed.form, 'B');
});

test('F6: le template ne vient pas du manifest ni du fichier expectations candidat', t => {
  const f = fixture(t), rules = [...qaRules, { action: 'shell', resource: '*', effect: 'allow' }];
  f.put('.opencode/tests/agentic/t05-qa/fixtures/expectations.json', { agents: qaIDs, permissions: rules });
  namedAgent(f, 'test-reviewer', qaText(rules));
  const result = run(f); has(result, 'PERMISSION_QA', 'FAIL'); assert.equal(result.status, 1);
  assert.deepEqual(qaCheck(result).observed.firstMismatch, firstRuleDifference(rules));
  f.manifest.template = { name: 'qa-readonly-declared-v1', permissions: rules }; f.save();
  const supplied = run(f); has(supplied, 'MANIFEST_SCHEMA', 'FAIL'); assert.equal(supplied.status, 1);
});

test('F6: template valide ne masque ni collision, ni schéma, ni FAIL prioritaire natif', t => {
  const f = fixture(t); namedAgent(f, 'test-reviewer', qaText());
  f.manifest.agents.push({ ...f.manifest.agents[0] }); f.save();
  const collision = run(f); assert.equal(collision.status, 1); has(collision, 'ID_COLLISION', 'FAIL');
  f.manifest.agents.pop(); f.save();
  f.put('.opencode/agents/test-reviewer.md', qaText().replace('description:', 'unexpected: value\ndescription:'));
  const schema = run(f); assert.equal(schema.status, 1); has(schema, 'SCHEMA_FIELD', 'FAIL');
  f.put('.opencode/agents/test-reviewer.md', qaText()); f.put('evidence.json', { schemaVersion: 1 });
  const native = run(f, 'native-evidence', ['--evidence', 'evidence.json']);
  assert.equal(native.status, 2); assertDeclaredTemplate(native, 'test-reviewer');
  has(native, 'NATIVE_AUTHENTICITY', 'INCONCLUSIVE'); has(native, 'CANARY_COVERAGE', 'INCONCLUSIVE');
  f.put('.opencode/agents/test-reviewer.md', qaText([...qaRules, { action: 'execute', resource: '*', effect: 'allow' }]));
  const failed = run(f, 'native-evidence', ['--evidence', 'evidence.json']);
  assert.equal(failed.status, 1); has(failed, 'PERMISSION_QA', 'FAIL'); has(failed, 'NATIVE_AUTHENTICITY', 'INCONCLUSIVE');
});

for (const env of ['env', 'ENV', 'Env', 'eNv', 'enV', 'ENv', 'EnV', 'eNV'])
  for (const suffix of ['', '.local', '.staging.backup']) test(`F1-R: suffixe production.${env}${suffix} reste secret`, t => {
    const f = fixture(t), resource = `docs/production.${env}${suffix}`;
    f.put(resource, 'inert secret sentinel');
    namedAgent(f, 'test-reviewer', agentText(`${denyAll}\n${rule('read', `*.${env}*`, 'deny')}\n${rule('read', resource)}`));
    const result = run(f); assert.equal(result.status, 1); has(result, 'PERMISSION_QA', 'FAIL');
    assert.deepEqual(qaCheck(result).observed.unsafe, [{ action: 'read', resource, effect: 'allow' }]);
  });

test('F1-R: forme A nominale docs/guide.md et suffixe non env préservés', t => {
  const f = fixture(t);
  for (const resource of ['docs/guide.md', 'docs/environment.md', 'docs/production.environment.md']) {
    f.put(resource, 'inert guide'); namedAgent(f, 'test-reviewer', agentText(`${denyAll}\n${rule('read', resource)}`));
    const result = run(f); assert.equal(result.status, 0); has(result, 'PERMISSION_QA', 'PASS');
  }
});

// Grammaire plain conservatrice : texte commence par lettre Unicode ou '_'.
// Décimaux/types core sont traités à part ; toute autre amorce exige des quotes.
for (const value of [',item', ',', '-.5', '+.5', '-.25e2', '+.25E-2', '.5', '+42', '01', '0x20', '1e3',
  '.Inf', '-.inf', '.NaN', '2026-10-16', '@name', '%name', '`name', '!tag', '&ref', '*ref',
  '[item]', '{item}', '|text', '>text', '-word', '?word', ':word', '/path', '.text'])
  test(`F2-R: classe plain/core bornée rejette ${value}, quotes préservées`, t => {
    const f = fixture(t);
    const put = text => f.put('.opencode/skills/example/SKILL.md', skillText('example').replace('Contrat de fixture inerte.', text));
    put(value); const result = run(f, 'syntax');
    assert.equal(result.status, 1); has(result, 'YAML_UNSUPPORTED', 'FAIL');
    for (const quoted of [JSON.stringify(value), `'${value}'`]) { put(quoted); assert.equal(run(f, 'schema').status, 0); }
  });

for (const value of ['0', '-42', '12.5', 'true', 'TRUE', 'False', 'null', 'NULL', '~'])
  test(`F2-R: scalaire typé supporté ${value}, jamais description texte`, t => {
    const f = fixture(t);
    f.put('.opencode/skills/example/SKILL.md', skillText('example').replace('Contrat de fixture inerte.', value));
    assert.equal(run(f, 'syntax').status, 0);
    const result = run(f, 'schema'); assert.equal(result.status, 1); has(result, 'SCHEMA_TYPE', 'FAIL');
  });

for (const value of ['Texte simple', 'État _initial avec 23 éléments.', '_identifier', 'alpha-beta/path', 'Texte, virgule interne.'])
  test(`F2-R: texte plain nominal ${value}`, t => {
    const f = fixture(t);
    f.put('.opencode/skills/example/SKILL.md', skillText('example').replace('Contrat de fixture inerte.', value));
    assert.equal(run(f, 'schema').status, 0);
  });
