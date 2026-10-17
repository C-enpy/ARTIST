import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

// Oracle T03 §3/5/6 + plan QA G-01..G-07, écrit avant les profils/skills.
// Contrôles textuels locaux uniquement : ni parser YAML complet ni routeur natif.
const root = fileURLToPath(new URL('../../../../', import.meta.url));
const agentIds = ['gpu-pipeline-engineer', 'shader-engineer', 'gpu-simulation-engineer'];
const skillIds = ['gpu-pipelines', 'gpu-shader-compilation', 'gpu-compute-simulation'];
const agentPath = id => `.opencode/agents/${id}.md`;
const skillPath = id => `.opencode/skills/${id}/SKILL.md`;
const references = [
  '.opencode/skills/gpu-pipelines/references/graphics.md',
  '.opencode/skills/gpu-pipelines/references/compute.md',
  '.opencode/skills/gpu-shader-compilation/references/compilation-abi.md',
  '.opencode/skills/gpu-compute-simulation/references/invariants.md',
];
const manifestPath = '.opencode/tests/agentic/t10-pipelines-shaders-simulation/manifest.json';
const manifest = JSON.parse(readFileSync(path.join(root, manifestPath), 'utf8'));

function read(relative) {
  assert.ok(existsSync(path.join(root, relative)), `Contrat T10 absent : ${relative}`);
  return readFileSync(path.join(root, relative), 'utf8').replaceAll('\r\n', '\n');
}

function frontmatter(text) {
  const match = /^---\n([\s\S]*?)\n---\n/.exec(text);
  assert.ok(match, 'Frontmatter requis');
  return match[1];
}

function requires(text, expressions, label) {
  for (const expression of expressions) assert.match(text, expression, `${label}: ${expression}`);
}

test('G-03/G-07 manifest explicite, IDs, obligations et allowlist bornée', () => {
  assert.equal(manifest.schemaVersion, 1);
  assert.equal(manifest.lot, 'T10');
  assert.deepEqual(manifest.agents.map(a => a.id), agentIds);
  assert.deepEqual(manifest.skills.map(s => s.id), skillIds);
  assert.deepEqual(manifest.obligations, ['G-01', 'G-02', 'G-03', 'G-04', 'G-05', 'G-06', 'G-07']);
  assert.deepEqual(manifest.agents.map(a => a.skills), [
    ['gpu-pipelines'],
    ['gpu-shader-compilation', 'artist-performance', 'gpu-backend-portability'],
    ['gpu-compute-simulation', 'gpu-pipelines'],
  ]);
  assert.deepEqual(manifest.agents.map(a => a.path), agentIds.map(agentPath));
  assert.deepEqual(manifest.skills.map(s => s.path), skillIds.map(skillPath));
  assert.deepEqual([...manifest.allowedPaths].sort(), [
    ...agentIds.map(agentPath), ...skillIds.map(skillPath), ...references, manifestPath,
    '.opencode/tests/agentic/t10-pipelines-shaders-simulation/contract.test.mjs',
    '.opencode/evidence/mission-gpu-qa/t10-result.md',
  ].sort());
});

for (const id of agentIds) {
  test(`G-03 ${id} : metadata V2 feuille, sans modèle imposé`, () => {
    const text = read(agentPath(id));
    assert.match(frontmatter(text), /^description: [^\n]+\nmode: subagent\npermissions:\n  - action: subagent\n    resource: "\*"\n    effect: deny$/);
    assert.doesNotMatch(frontmatter(text), /^(model|variant|tools|permission|temperature):/m);
    requires(text, [/AGENTS\.md/, /DEFER_TO/, /lead/, /feuille/, /INCONCLUSIVE/], id);
  });
}

for (const id of skillIds) {
  test(`G-02/G-03 ${id} : skill ciblé et procédure vérifiable`, () => {
    const text = read(skillPath(id));
    assert.match(frontmatter(text), new RegExp(`^name: ${id}\\ndescription: [^\\n]+$`));
    assert.ok(text.split('\n').length < 180, 'Entrée courte, détails dans références');
    requires(text, [
      /## Déclenchement et exclusions/, /## Préconditions et pin ARTIST/,
      /## Procédure/, /## Oracles et réussite/, /## Transfert/,
      /OpenGL/, /3\.3 core/, /INCONCLUSIVE/, /DEFER_TO/,
    ], id);
  });
}

test('G-03 liens requis explicites ; existence limitée aux fichiers T10', () => {
  assert.equal(manifest.requiredLinks.length, 10);
  for (const { from, to } of manifest.requiredLinks) {
    const text = read(from);
    const relative = path.posix.relative(path.posix.dirname(from), to);
    assert.ok(text.includes(`](${relative})`), `Lien requis ${from} -> ${to}`);
    if (manifest.allowedPaths.includes(to)) read(to);
    // Les voisins appartiennent aux autres lots ; leur existence relève de T13.
  }
});

test('G-02/G-05 graphics : interface, états, image et mismatch négatif', () => {
  requires(read(references[0]), [
    /vertex input/, /stride/, /offset/, /attachments/, /depth/, /blend/,
    /raster/, /sample count/, /stage/, /entry/, /sRGB/, /Nominal/,
    /Négatif/, /triangle/, /incompatible/, /image/, /INCONCLUSIVE/,
  ], 'graphics');
});

test('G-02/G-05 compute : procédure distincte, bornes et consommation', () => {
  requires(read(references[1]), [
    /workgroup/, /dispatch/, /ceil\(N \/ L\)/, /N = 257/, /N = 0/,
    /limites/, /binding/, /sentinelle/, /barrier/, /visibilité/,
    /dernière consommation/, /Négatif/, /OpenGL 4\.3/, /3\.3/,
  ], 'compute');
});

test('G-02/G-05 compilation : source, cible, ABI et cache non universel', () => {
  requires(read(references[2]), [
    /Slang/, /HLSL/, /GLSL/, /MSL/, /SPIR-V/, /DXIL/, /réflexion/,
    /stage/, /entry/, /ABI/, /row-major/, /column-major/, /alignement/,
    /offset/, /stride/, /bool/, /float3/, /cache/, /permutations/,
    /bornées/, /Nominal/, /Négatif/, /shader compilé ≠ pipeline valide/,
    /Windows/, /Apple/, /INCONCLUSIVE/,
  ], 'compilation ABI');
});

test('G-02/G-05 simulation : quatre domaines, invariants et stabilité', () => {
  requires(read(references[3]), [
    /particules/, /cloth/, /hair/, /fluides/, /seed/, /dt/, /NaN/,
    /bornes/, /invariants/, /CFL/, /divergence/, /pinned/, /tolérance/,
    /queues/, /rendu/, /Nominal/, /Négatif/, /toutes les queues/,
  ], 'simulation');
});

test('G-02 frontière binding time, ownership et pas de type par contenu', () => {
  requires(read(skillPath('gpu-pipelines')), [
    /compile/, /link/, /cook/, /runtime/, /unique_ptr/, /emprunt/,
    /génération/, /complétion/, /par lots/, /pas un type par/,
  ], 'binding times');
  requires(read(skillPath('gpu-compute-simulation')), [
    /état runtime/, /pas un type par émetteur/, /gpu-resource-engineer/,
    /render-graph-engineer/, /gpu-portability-engineer/,
  ], 'simulation handoff');
});

test('G-04 cas 1 : graphics Vulkan depth, sans charger toutes les API', () => {
  requires(read(agentPath('gpu-pipeline-engineer')), [
    /graphics Vulkan.*depth/, /gpu-pipelines/, /vulkan-backend/,
    /API.*absente/, /clarif/, /Ne charge pas tous/,
  ], 'cas 1');
});

test('G-04 cas 2 : HLSL coûteux, performance avant transformation', () => {
  requires(read(agentPath('shader-engineer')), [
    /HLSL coûteux/, /artist-performance.*puis.*gpu-shader-compilation/,
    /baseline/, /cible/, /branche supprimée.*gain/,
  ], 'cas 2');
});

test('G-04 cas 4 : shader Vulkan vers Metal, compilation et portabilité', () => {
  requires(read(agentPath('shader-engineer')), [
    /Vulkan vers Metal/, /gpu-shader-compilation.*gpu-backend-portability/,
    /Metal natif.*ABI.*capacités/, /Slang.*adoption/,
  ], 'cas 4');
});

test('G-04 cas 5 : particules compute, clarification API et contraintes', () => {
  requires(read(agentPath('gpu-simulation-engineer')), [
    /particules compute/, /gpu-compute-simulation.*gpu-pipelines/,
    /API/, /contraintes/, /clarif/, /CPU seul.*cpp-engineer/,
  ], 'cas 5');
});

test('G-01/G-06 références originales, sources officielles et limites', () => {
  for (const relative of references) {
    const text = read(relative);
    requires(text, [/## Sources et limites/, /https:\/\//, /original/, /licence/, /INCONCLUSIVE/], relative);
  }
  requires(read(skillPath('gpu-shader-compilation')), [
    /artist-performance/, /A\/B/, /qualité/, /dispersion/, /compilateur/,
  ], 'optimisation mesurée');
});

test('G-03 négatif : frontmatter malformé rejeté, pas YAML complet revendiqué', () => {
  assert.throws(() => frontmatter('description: exemple\n'), /Frontmatter requis/);
});

test('G-03 négatif : une permission subagent allow ne passe pas le contrat', () => {
  const text = read(agentPath('shader-engineer'));
  const mutated = text.replace('effect: deny', 'effect: allow');
  assert.notEqual(mutated, text, 'Mutation appliquée');
  assert.throws(() => assert.match(frontmatter(mutated), /effect: deny$/), assert.AssertionError);
});

test('G-05 négatif : enlever la distinction shader/pipeline est détecté', () => {
  const text = read(references[2]);
  const mutated = text.replaceAll('shader compilé ≠ pipeline valide', 'pipeline validé par compilation');
  assert.notEqual(mutated, text, 'Mutation appliquée');
  assert.throws(() => requires(mutated, [/shader compilé ≠ pipeline valide/], 'ABI'), assert.AssertionError);
});

test('G-05 négatif : enlever le contrôle sentinelle compute est détecté', () => {
  const text = read(references[1]);
  const mutated = text.replaceAll('sentinelle', 'témoin non vérifié');
  assert.notEqual(mutated, text, 'Mutation appliquée');
  assert.throws(() => requires(mutated, [/sentinelle/], 'compute'), assert.AssertionError);
});
