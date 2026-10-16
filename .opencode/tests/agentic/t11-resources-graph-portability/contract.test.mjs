import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

// Oracle figé AVANT les contenus : T03 §3/4/5/6, QA G-01..07 et T-07.
// Contrôles de documents seulement. Aucun chargement natif ni modèle de GPU.
const root = fileURLToPath(new URL('../../../../', import.meta.url));
const manifest = JSON.parse(readFileSync(new URL('./manifest.json', import.meta.url), 'utf8'));
const agentIds = ['gpu-resource-engineer', 'render-graph-engineer', 'gpu-portability-engineer'];
const skillIds = ['gpu-resource-lifecycle', 'gpu-render-graph', 'gpu-capability-detection', 'gpu-backend-portability'];
const referencePaths = [
  '.opencode/skills/gpu-resource-lifecycle/references/ledger-and-hazards.md',
  '.opencode/skills/gpu-render-graph/references/graph-contract.md',
  '.opencode/skills/gpu-capability-detection/references/api-capabilities.md',
  '.opencode/skills/gpu-backend-portability/references/conventions.md',
];
const agentPath = id => `.opencode/agents/${id}.md`;
const skillPath = id => `.opencode/skills/${id}/SKILL.md`;
const read = relative => {
  const absolute = path.join(root, relative);
  assert.ok(existsSync(absolute), `Contrat T11 absent : ${relative}`);
  return readFileSync(absolute, 'utf8').replaceAll('\r\n', '\n');
};
const requireTerms = (text, terms) => {
  for (const term of terms) assert.ok(text.includes(term), `Clause obligatoire absente : ${term}`);
};
const metadata = text => {
  const match = /^---\n([\s\S]*?)\n---\n/.exec(text);
  assert.ok(match, 'Frontmatter T11 absent ou non fermé');
  return match[1];
};
const assertAgent = text => {
  const front = metadata(text);
  assert.match(front, /^description: .+$/m);
  assert.match(front, /^mode: subagent$/m);
  assert.match(front, /^permissions:\n  - action: subagent\n    resource: "\*"\n    effect: deny$/m);
  assert.doesNotMatch(front, /^(model|variant|temperature|tools):/m);
  // Pas de parser V2 général ni garantie de confinement : forme locale volontairement étroite.
  assert.equal(front.split('\n').length, 6, 'Champs/règles supplémentaires à faire réexaminer');
  requireTerms(text, ['## Sélection et limites', '## Méthode', '## Pin ARTIST', '## Retour', 'DEFER_TO', 'INCONCLUSIVE']);
  assert.doesNotMatch(text, /team_spawn\s*\(|activate_project\s*\(/);
};
const assertSkill = (text, id) => {
  const front = metadata(text);
  assert.match(front, new RegExp(`^name: ${id}$`, 'm'));
  assert.match(front, /^description: .+$/m);
  assert.equal(front.split('\n').length, 2, 'Métadonnées locales name/description seulement');
  requireTerms(text, ['## Déclencheurs et exclusions', '## Préconditions et outils', '## Procédure', '## Nominal et négatif', '## Pièges', '## Réussite et preuves', '## Références et transfert', 'INCONCLUSIVE', 'DEFER_TO']);
  assert.ok(text.split('\n').length < 160, 'Skill principal trop long : extraire une référence ciblée');
  assert.doesNotMatch(text, /\.cursor\/|disable-model-invocation:/);
};

test('G-02/G-07 : manifest explicite, trois profils et quatre skills exacts', () => {
  assert.equal(manifest.schemaVersion, 1);
  assert.equal(manifest.lot, 'T11');
  assert.deepEqual(manifest.agents.map(a => a.id), agentIds);
  assert.deepEqual(manifest.skills.map(s => s.id), skillIds);
  assert.deepEqual(manifest.obligations, ['G-01', 'G-02', 'G-03', 'G-04', 'G-05', 'G-06', 'G-07', 'T-07']);
  assert.deepEqual(manifest.agents.map(a => a.skills), [
    ['gpu-resource-lifecycle', 'd3d12-backend'],
    ['gpu-render-graph', 'artist-static-boundary'],
    ['gpu-capability-detection', 'gpu-backend-portability', 'artist-gpu-debug'],
  ]);
  assert.deepEqual(manifest.allowedPaths, [
    ...agentIds.map(agentPath),
    ...skillIds.flatMap((id, i) => [skillPath(id), referencePaths[i]]),
    '.opencode/tests/agentic/t11-resources-graph-portability/manifest.json',
    '.opencode/tests/agentic/t11-resources-graph-portability/contract.test.mjs',
    '.opencode/evidence/mission-gpu-qa/t11-result.md',
  ]);
  manifest.agents.forEach(a => assert.equal(a.path, agentPath(a.id)));
  manifest.skills.forEach(s => assert.equal(s.path, skillPath(s.id)));
});

for (const id of agentIds) test(`G-03 : format et rôle feuille ${id}`, () => assertAgent(read(agentPath(id))));
for (const id of skillIds) test(`G-02/G-03 : contrat de skill ${id}`, () => assertSkill(read(skillPath(id)), id));

test('G-03 : liens requis explicites, cibles voisines différées à intégration', () => {
  assert.equal(manifest.requiredLinks.length, 11);
  for (const { from, to } of manifest.requiredLinks) {
    const text = read(from);
    const targets = [...text.matchAll(/\[[^\]]+\]\(([^)]+)\)/g)].map(m => path.posix.normalize(path.posix.join(path.posix.dirname(from), m[1])));
    assert.ok(targets.includes(to), `Lien requis absent : ${from} -> ${to}`);
    if (manifest.allowedPaths.includes(to)) read(to);
    // Présence des IDs des autres lots n'est PAS testée dans ce worktree isolé.
  }
});

test('G-02/G-04 : principal et skills minimum des scénarios 3/8/11/12', () => {
  requireTerms(read(agentPath(agentIds[0])), ['Scénario 3', 'gpu-resource-lifecycle', 'd3d12-backend', 'RAW', 'WAR', 'WAW']);
  requireTerms(read(agentPath(agentIds[1])), ['Scénario 8', 'gpu-render-graph', 'artist-static-boundary', 'authoring', 'cook', 'runtime-frame']);
  requireTerms(read(agentPath(agentIds[2])), ['Scénario 11', 'Scénario 12', 'gpu-capability-detection', 'gpu-backend-portability', 'artist-gpu-debug', 'RTX']);
});

test('G-02/G-05 : ledger, hazards et retrait toutes queues explicités', () => {
  const text = read(referencePaths[0]);
  requireTerms(text, ['owner', 'borrow', 'génération', 'sous-ressource', 'alignement', 'résidence', 'descripteur', 'RAW', 'WAR', 'WAW', 'visibilité', 'attente CPU', 'toutes les queues', 'La génération ne constitue pas une attente GPU', 'completed[q] >= lastUse[q]', 'contexte', 'glMemoryBarrier', 'glFenceSync', 'Signal', 'Wait', 'vkCmdPipelineBarrier2', 'MTLFence']);
});

test('G-02/G-05 : graphes distincts, négatifs, aliasing et invalidation', () => {
  const text = read(referencePaths[1]);
  requireTerms(text, ['authoring', 'cook immuable', 'runtime-frame', 'cycle', 'read-before-produce', 'shadow', 'post', 'intervalle', 'happens-before', 'aliasing', 'rebuild', 'invalidation', 'source/options/ABI', 'retirer', 'fusion', 'async compute', 'import', 'export']);
});

test('G-05/G-06 : capacités quatre API sans inférence matérielle', () => {
  const text = read(referencePaths[2]);
  requireTerms(text, ['Matrice conceptuelle', 'OpenGL', 'Vulkan', 'D3D12', 'Metal', 'supporté', 'activé', 'compatible', 'inconnu', 'limites', 'format', 'fallback', 'vkGetPhysicalDeviceFeatures2', 'vkGetPhysicalDeviceFormatProperties2', 'CheckFeatureSupport', 'MESH_SHADER_TIER', 'supportsFamily', 'glGetStringi', 'RTX', 'NOT_AVAILABLE', 'INCONCLUSIVE', 'OpenGL 3.3']);
});

test('G-05/G-06 : conventions, qualité et écarts non représentables', () => {
  const text = read(referencePaths[3]);
  requireTerms(text, ['Matrice conceptuelle', 'clip', '[-w,+w]', '[0,w]', 'handedness', 'winding', 'row-major', 'sRGB', 'linéaire', 'stride', 'offset', 'non représentable', 'fallback', 'plus petit dénominateur', 'tolérance', 'image', 'pass']);
});

test('G-01 : références officielles et rédaction originale, aucune licence déduite', () => {
  for (const file of referencePaths) {
    const text = read(file);
    requireTerms(text, ['Rédaction originale', 'licence', 'https://', '2026-10-16']);
    assert.doesNotMatch(text, /licence (MIT|Apache|BSD) appliquée/);
  }
});

test('G-06/T-07 : pin observé, pas adoption ni backend annoncé', () => {
  for (const id of agentIds) {
    const text = read(agentPath(id));
    requireTerms(text, ['40a7cc3', 'OpenGL', 'unique_ptr', 'emprunt', 'Proposed', 'partial', 'Windows Debug', 'Pin observé']);
  }
  for (const id of skillIds) requireTerms(read(skillPath(id)), ['OpenGL', 'exécut', 'INCONCLUSIVE']);
});

// Mutations inertes : elles testent les assertions locales, jamais le runtime OpenCode.
test('G-03 négatif : mode primary refusé pour un profil feuille', () => {
  const text = read(agentPath(agentIds[0]));
  assert.throws(() => assertAgent(text.replace('mode: subagent', 'mode: primary')), assert.AssertionError);
});
test('G-03 négatif : autorisation de sous-agent refusée', () => {
  const text = read(agentPath(agentIds[0]));
  assert.throws(() => assertAgent(text.replace('effect: deny', 'effect: allow')), assert.AssertionError);
});
test('G-03 négatif : modèle imposé refusé', () => {
  const text = read(agentPath(agentIds[0]));
  assert.throws(() => assertAgent(text.replace('mode: subagent', 'mode: subagent\nmodel: forbidden')), assert.AssertionError);
});
test('G-02 négatif : skill sans procédure refusé', () => {
  const text = read(skillPath(skillIds[0]));
  assert.throws(() => assertSkill(text.replace('## Procédure', '## Autre'), skillIds[0]), assert.AssertionError);
});
test('G-02 négatif : skill sans négatif refusé', () => {
  const text = read(skillPath(skillIds[1]));
  assert.throws(() => assertSkill(text.replace('## Nominal et négatif', '## Exemple'), skillIds[1]), assert.AssertionError);
});
test('G-03 négatif : frontmatter malformé refusé', () => {
  const text = read(skillPath(skillIds[2]));
  assert.throws(() => assertSkill(text.replace('---\n', '--\n'), skillIds[2]), assert.AssertionError);
});
