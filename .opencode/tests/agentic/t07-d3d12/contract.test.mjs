import assert from 'node:assert/strict';
import { existsSync, readFileSync, readdirSync } from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

// Oracle fixé AVANT les documents : qa-plan G-01..07/T-07, contrat T03 §3-6,
// brief T07. Contrôles de présence/cohérence, PAS un parseur V2 ni une preuve GPU.
// Aucun import T04, appel réseau, commande shell ou exécution de fixture.
const root = fileURLToPath(new URL('../../../../', import.meta.url));
const agentPath = '.opencode/agents/d3d12-backend-engineer.md';
const skillPath = '.opencode/skills/d3d12-backend/SKILL.md';
const diagnosticPath = '.opencode/skills/d3d12-backend/references/diagnostic.md';
const sourcesPath = '.opencode/skills/d3d12-backend/references/sources.md';
const manifestPath = '.opencode/tests/agentic/t07-d3d12/manifest.json';
const allowedPaths = [
  agentPath, skillPath, diagnosticPath, sourcesPath,
  '.opencode/tests/agentic/t07-d3d12/contract.test.mjs', manifestPath,
  '.opencode/evidence/mission-gpu-qa/t07-result.md',
];

function read(relative) {
  const absolute = path.join(root, relative);
  assert.ok(existsSync(absolute), `Artefact contractuel non livré : ${relative}`);
  return readFileSync(absolute, 'utf8');
}

function requireTerms(text, terms) {
  for (const term of terms) assert.ok(text.includes(term), `Clause absente : ${term}`);
}

test('G-02/G-07 : manifest explicite, IDs et allowlist T07 fermée', () => {
  const manifest = JSON.parse(read(manifestPath));
  assert.deepEqual(Object.keys(manifest).sort(), [
    'schemaVersion', 'lot', 'agents', 'skills', 'obligations', 'requiredLinks', 'allowedPaths',
  ].sort());
  assert.equal(manifest.schemaVersion, 1);
  assert.equal(manifest.lot, 'T07');
  assert.deepEqual(manifest.agents, [{
    id: 'd3d12-backend-engineer', path: agentPath,
    skills: ['d3d12-backend', 'artist-build-validation', 'artist-gpu-debug'],
  }]);
  assert.deepEqual(manifest.skills, [{ id: 'd3d12-backend', path: skillPath }]);
  assert.deepEqual(manifest.obligations,
    ['G-01', 'G-02', 'G-03', 'G-04', 'G-05', 'G-06', 'G-07', 'T-07']);
  assert.deepEqual(manifest.allowedPaths, allowedPaths);
  assert.deepEqual(manifest.requiredLinks, [
    { from: agentPath, to: skillPath },
    { from: skillPath, to: diagnosticPath },
    { from: skillPath, to: sourcesPath },
  ]);
});

test('G-03 : profil subagent V2 sans modèle et refus de sous-agent', () => {
  const agent = read(agentPath);
  assert.match(agent, /^---\r?\ndescription: [^\r\n]+\r?\nmode: subagent\r?\npermissions:\r?\n  - action: subagent\r?\n    resource: "\*"\r?\n    effect: deny\r?\n---\r?\n/);
  assert.doesNotMatch(agent, /^(?:model|variant|tools|permission):/m);
  requireTerms(agent, ['agent feuille', 'DEFER_TO', 'lead', 'sans sous-délégation']);
});

test('G-03/G-04 : skill natif local, description et divulgation progressive', () => {
  const skill = read(skillPath);
  assert.match(skill, /^---\r?\nname: d3d12-backend\r?\ndescription: [^\r\n]+\r?\n---\r?\n/);
  assert.ok(skill.split('\n').length < 160, 'Entrée légère, détails en références');
  assert.doesNotMatch(skill, /\.cursor\/|^disable-model-invocation:|^model:/m);
  requireTerms(skill, ['## Déclencheurs et exclusions', '## Préconditions',
    '## Procédure', '## Preuves et retour']);
});

test('G-03 : liens locaux explicites résolus, sans dépendance à un futur lot', () => {
  for (const { from, to } of JSON.parse(read(manifestPath)).requiredLinks) {
    assert.ok(allowedPaths.includes(from) && allowedPaths.includes(to));
    const targets = [...read(from).matchAll(/\]\(([^)]+)\)/g)]
      .filter(([, href]) => !href.startsWith('https://'))
      .map(([, href]) => path.posix.normalize(path.posix.join(path.posix.dirname(from), href)));
    assert.ok(targets.includes(to), `Lien absent : ${from} -> ${to}`);
    assert.ok(read(to).length > 100);
  }
});

test('G-04 : scénario 3 ressources principal, DDS seul non déclencheur API', () => {
  const skill = read(skillPath);
  requireTerms(skill, ['Scénario 3', 'principal `gpu-resource-engineer`',
    '`gpu-resource-lifecycle` + `d3d12-backend`',
    'DDS seul ne sélectionne pas D3D12', '`asset-engineer`',
    'API absente', 'clarification', 'DEFER_TO']);
});

test('G-02/G-05 : pin réel, séparation compile/cook/runtime et non-adoption ADR', () => {
  const skill = read(skillPath);
  requireTerms(skill, ['OpenGL', 'expertise', 'pas un backend D3D12 implémenté',
    'unique_ptr', 'getPass', 'emprunt', 'Proposed', 'partial', 'Windows Debug',
    'compile', 'cook', 'runtime', 'DXC 1.8', 'ni device ni frame']);
});

test('G-02 : binding D3D12 et layout CPU/GPU, pas simple catalogue de noms', () => {
  requireTerms(read(diagnosticPath), ['CheckFeatureSupport', 'Root Signature 1.1',
    'register', 'space', 'SetDescriptorHeaps', 'CBV_SRV_UAV', 'SAMPLER',
    'RTV/DSV', 'SetGraphicsRoot', 'SetComputeRoot', 'PSO', 'DXIL',
    'GetCopyableFootprints', '256', '512', 'stride', 'sRGB', 'handedness']);
});

const lifetimeClauses = [
  'ID3D12CommandAllocator::Reset', 'ID3D12GraphicsCommandList::Reset',
  'toutes les queues', 'GetCompletedValue', 'SetEventOnCompletion',
  'Signal', 'Wait', 'génération', 'UINT64_MAX',
];
test('G-02/G-05 : allocateur/list distincts, retraite GPU et ownership explicites', () => {
  requireTerms(read(diagnosticPath), [...lifetimeClauses,
    'command list fermée', 'CPU', 'en vol', 'COM', 'descriptor',
    'EnqueueMakeResident', 'Evict', 'budget']);
});

const barrierClauses = [
  'EnhancedBarriersSupported', 'D3D12_FEATURE_D3D12_OPTIONS12',
  'ID3D12GraphicsCommandList7', 'SyncBefore', 'SyncAfter', 'AccessBefore',
  'AccessAfter', 'LayoutBefore', 'LayoutAfter', 'fallback legacy',
];
test('G-02/G-05 : transitions/UAV/aliasing et Enhanced Barriers conditionnelles', () => {
  requireTerms(read(diagnosticPath), [...barrierClauses, 'RAW/WAR/WAW',
    'subresource', 'UAV', 'aliasing', 'COMMON', 'promotion', 'decay',
    'ne bloque pas le CPU', 'ne remplace pas', 'ressources']);
});

test('G-05/G-06 : reproduction et oracles nominaux/négatifs, debug conditionnel', () => {
  requireTerms(read(diagnosticPath), ['D12-N1', 'D12-X1', 'D12-X2', 'D12-X3',
    'D12-X4', 'D12-X5', '64 × 64', 'R8G8B8A8_UNORM', 'frame 0',
    '255, 0, 0, 255', 'avant la création du device', 'GBV', 'InfoQueue',
    'INCONCLUSIVE', 'pas une mesure de performance']);
});

test('G-01 : sources officielles et limites de réutilisation', () => {
  requireTerms(read(sourcesPath), ['learn.microsoft.com', 'DirectX-Graphics-Samples',
    'DirectXShaderCompiler', 'DirectX-Headers', 'D3D12MemoryAllocator',
    'DirectXTex', 'DirectXMesh', 't02-external-sources.md',
    'arbre SHA', 'commit', 'notice', 'liens seulement', 'rédaction originale']);
});

test('G-06/T-07 : preuves qualifiées et outils non présumés', () => {
  requireTerms(read(skillPath), ['HEAD', 'hashes', 'delta', 'PASS', 'FAIL',
    'INCONCLUSIVE', 'zéro test', 'PIX', 'aucun MCP', 'installation',
    'mesuré', 'estimé', 'chargement natif', 'permissions']);
});

test('G-07 : skill documentaire uniquement, aucun exécutable caché', () => {
  const base = path.join(root, '.opencode/skills/d3d12-backend');
  assert.ok(existsSync(base), 'Répertoire skill non livré');
  const files = readdirSync(base, { recursive: true, withFileTypes: true })
    .filter(entry => entry.isFile())
    .map(entry => path.relative(base, path.join(entry.parentPath, entry.name)).replaceAll('\\', '/'));
  assert.deepEqual(files.sort(), ['SKILL.md', 'references/diagnostic.md', 'references/sources.md']);
});

// Mutations en mémoire seulement : prouvent la sensibilité du garde textuel,
// ni la véracité technique des phrases ni le rejet d'une commande par D3D12.
for (const [name, terms] of [['lifetime', lifetimeClauses], ['barriers', barrierClauses]]) {
  test(`Oracle négatif ${name} : chaque clause retirée est détectée`, () => {
    const candidate = terms.join('\n');
    assert.doesNotThrow(() => requireTerms(candidate, terms));
    for (const term of terms) {
      assert.throws(() => requireTerms(candidate.replace(term, ''), terms),
        { code: 'ERR_ASSERTION', message: `Clause absente : ${term}` });
    }
  });
}
