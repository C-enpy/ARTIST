import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

// Oracle fixé par qa-plan G-01..G-07/T-07 et t03-contract §3/4/6.
// Présence statique seulement : ni sélection native, ni permission effective,
// ni compilateur Apple, ni contrat GPU ne sont évalués par ces expressions.
const root = fileURLToPath(new URL('../../../../', import.meta.url));
const agentPath = '.opencode/agents/metal-backend-engineer.md';
const skillPath = '.opencode/skills/metal-backend/SKILL.md';
const proceduresPath = '.opencode/skills/metal-backend/references/procedures.md';
const sourcesPath = '.opencode/skills/metal-backend/references/sources.md';
const manifestPath = '.opencode/tests/agentic/t08-metal/manifest.json';

function read(relativePath) {
  try {
    return readFileSync(path.join(root, relativePath), 'utf8');
  } catch (error) {
    if (error.code === 'ENOENT') return '';
    throw error;
  }
}

const agent = read(agentPath);
const skill = read(skillPath);
const procedures = read(proceduresPath);
const sources = read(sourcesPath);
const manifest = JSON.parse(read(manifestPath));

// Ces exigences sont explicites, non extraites du manifest ou du skill testé.
const obligations = ['G-01', 'G-02', 'G-03', 'G-04', 'G-05', 'G-06', 'G-07', 'T-07'];
const allowedPaths = [agentPath, skillPath, proceduresPath, sourcesPath,
  '.opencode/tests/agentic/t08-metal/contract.test.mjs', manifestPath,
  '.opencode/evidence/mission-gpu-qa/t08-result.md'];

test('G-03/G-07 : manifest borné à T08, IDs et obligations explicites', () => {
  assert.equal(manifest.schemaVersion, 1);
  assert.equal(manifest.lot, 'T08');
  assert.deepEqual(manifest.agents, [{id: 'metal-backend-engineer', path: agentPath,
    skills: ['metal-backend', 'artist-build-validation']}]);
  assert.deepEqual(manifest.skills, [{id: 'metal-backend', path: skillPath}]);
  assert.deepEqual(manifest.obligations, obligations);
  assert.deepEqual(manifest.allowedPaths, allowedPaths);
});

test('G-03 : profil V2 feuille, sans modèle ni champ Cursor', () => {
  assert.ok(agent.length > 0, `profil requis absent : ${agentPath}`);
  assert.match(agent, /^---\r?\n/);
  assert.match(agent, /^description: .+Metal.+$/m);
  assert.match(agent, /^mode: subagent$/m);
  assert.match(agent, /permissions:\r?\n  - action: subagent\r?\n    resource: "\*"\r?\n    effect: deny/);
  const frontmatter = agent.split(/^---\s*$/m)[1];
  assert.doesNotMatch(frontmatter, /^(model|variant|tools|alwaysApply|globs):/m);
  assert.match(agent, /DEFER_TO/);
  assert.match(agent, /pas de sous-délégation/);
});

test('G-03/G-04 : skill V2 et progressive disclosure sélective', () => {
  assert.match(skill, /^---\r?\nname: metal-backend\r?\ndescription: .+\r?\n---/);
  assert.match(skill, /## Déclencheurs et exclusions/);
  assert.match(skill, /shader-engineer/);
  assert.match(skill, /sans adoption de Slang/);
  assert.ok(skill.split('\n').length < 200, 'entrée légère, détails en références');
});

test('G-03 : liens attendus écrits ; IDs futurs sans contrôle global prématuré', () => {
  const links = [
    [agentPath, skillPath], [skillPath, proceduresPath], [skillPath, sourcesPath],
    [skillPath, '.opencode/skills/gpu-shader-compilation/SKILL.md'],
    [skillPath, '.opencode/skills/gpu-resource-lifecycle/SKILL.md'],
    [skillPath, '.opencode/skills/gpu-backend-portability/SKILL.md'],
    [skillPath, '.opencode/skills/artist-build-validation/SKILL.md'],
  ];
  assert.deepEqual(manifest.requiredLinks, links.map(([from, to]) => ({from, to})));
  for (const [from, to] of links) {
    const relative = path.posix.relative(path.posix.dirname(from), to);
    assert.ok(read(from).includes(`](${relative})`), `${from} -> ${to}`);
  }
});

test('G-02/T-07 : pin honnête OpenGL, unique_ptr/emprunt, ADR inchangées', () => {
  assert.match(skill, /OpenGL/);
  assert.match(skill, /unique_ptr/);
  assert.match(skill, /emprunt/);
  assert.match(skill, /0001\/0002\/0004\/0006\/0007.*Proposed/);
  assert.match(skill, /0003\/0005.*partial/);
  assert.match(skill, /0008.*Windows/);
  assert.match(skill, /HEAD.*hashes.*delta/);
});

test('G-05 : gates distincts SDK, OS, famille, features et fallback', () => {
  assert.match(procedures, /## P1 — Capacités/);
  for (const token of ['SDK', 'OS', 'supportsFamily', 'Metal 3', 'Metal 4', 'fallback', 'N-FAMILY']) {
    assert.ok(procedures.includes(token), `P1 : ${token}`);
  }
  assert.match(procedures, /non résolu.*PATH.*absence absolue/);
});

test('G-02/G-05 : ownership CPU distinct de complétion GPU et résidence', () => {
  for (const token of ['retain', 'release', 'autorelease', 'NS::TransferPtr', 'NS::RetainPtr',
    'MTL4CommandAllocator', 'retainedReferences', 'MTLResidencySet', 'N-INFLIGHT']) {
    assert.ok(procedures.includes(token), `lifetime : ${token}`);
  }
  assert.match(procedures, /génération.*ne.*complétion/);
  assert.match(procedures, /toutes les queues/);
  assert.match(procedures, /ressources en vol/);
});

test('G-02 : synchronisation propre à Metal 4, pas barrière universelle', () => {
  assert.match(procedures, /hazardTrackingMode.*sans effet.*MTL4CommandQueue/);
  for (const token of ['RAW', 'WAR', 'WAW', 'MTLFence', 'MTLEvent', 'MTLSharedEvent',
    'ordre', 'visibilité', 'attente CPU', 'useResource', 'useHeap']) {
    assert.ok(procedures.includes(token), `synchronisation : ${token}`);
  }
});

test('G-05 : ABI source -> pipeline -> image, cas négatif discriminant', () => {
  for (const token of ['N-ABI', 'stride', 'offset', 'alignement', 'float3', 'packed_float3',
    'MTLVertexDescriptor', 'buffer(0)', 'sampleCount', 'sRGB', 'depth', 'handedness']) {
    assert.ok(procedures.includes(token), `ABI : ${token}`);
  }
  assert.match(procedures, /MSL généré.*Windows.*ne prouve/);
  assert.match(procedures, /xcrun --sdk macosx metal/);
  assert.match(procedures, /xcrun --sdk macosx metallib/);
});

test('G-02/G-06 : drawable, présentation et capture Apple conditionnelle', () => {
  for (const token of ['nextDrawable', 'presentDrawable', 'waitForDrawable', 'signalDrawable',
    'present()', 'MTLCaptureManager', 'Xcode', 'RenderDoc', 'INCONCLUSIVE']) {
    assert.ok(procedures.includes(token), `présentation : ${token}`);
  }
  assert.match(procedures, /drawable nul/);
});

test('G-05/G-06 : nominal, négatifs et preuves non fusionnés', () => {
  for (const token of ['NOMINAL', 'N-INFLIGHT', 'N-FAMILY', 'N-ABI', 'Attendu', 'Oracle', 'Intent']) {
    assert.ok(procedures.includes(token), `scénarios : ${token}`);
  }
  assert.match(skill, /offline/);
  assert.match(skill, /natif/);
  assert.match(skill, /GPU/);
  assert.match(skill, /image/);
  assert.match(skill, /performance/);
  assert.match(skill, /zéro test/);
});

test('G-01/G-07 : sources originales, licence et limites de réutilisation', () => {
  for (const token of ['https://developer.apple.com/metal/cpp/',
    'https://developer.apple.com/documentation/metal/resource-synchronization',
    'https://opencode.ai/v2/docs/agents', 'rédaction originale', 'arbre SHA',
    'commit', 'notice', 'licence inconnue', 'aucune installation']) {
    assert.ok(sources.includes(token), `provenance : ${token}`);
  }
});

// Mutations inertes en mémoire : elles prouvent la discrimination de ces
// contrôles textuels, pas l'interdiction d'une action native ni un bug driver.
const negativeCases = [
  ['N-FAMILY', /supportsFamily/, 'famille non vérifiée'],
  ['N-INFLIGHT', /toutes les queues/, 'seule queue consultée'],
  ['N-ABI', /packed_float3/, 'packing omis'],
  ['Metal4-hazard', /hazardTrackingMode.*sans effet.*MTL4CommandQueue/, 'tracking implicite supposé'],
];
for (const [id, requirement, replacement] of negativeCases) {
  test(`oracle statique négatif ${id} : suppression détectée sans toucher les fichiers`, () => {
    assert.match(procedures, requirement, 'nominal documentaire présent avant mutation');
    const mutant = procedures.replace(new RegExp(requirement.source, 'g'), replacement);
    assert.throws(() => assert.match(mutant, requirement), assert.AssertionError);
  });
}
