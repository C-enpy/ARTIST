import assert from 'node:assert/strict';
import { test } from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { createHash } from 'node:crypto';
import { spawnSync } from 'node:child_process';

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
  const gitRun = args => {
    const run = spawnSync('git', ['--no-optional-locks', '-C', root, ...args], { encoding: 'utf8' });
    assert.equal(run.status, 0, `précondition Git fixture : ${run.stderr}`);
    return run.stdout.trim();
  };
  if (git) {
    gitRun(['init', '--quiet']);
    gitRun(['add', '.']);
    gitRun(['-c', 'user.name=T04 fixture', '-c', 'user.email=t04@example.invalid',
      '-c', 'commit.gpgsign=false', 'commit', '--quiet', '-m', 'inert fixture']);
  }
  return { root, put, manifest, save, gitRun };
}

function run(f, mode = 'offline', extra = []) {
  const args = [script, '--mode', mode, '--root', f.root, '--manifest', 'manifest.json', ...extra];
  const result = spawnSync(process.execPath, args, { encoding: 'utf8', timeout: 20000 });
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
