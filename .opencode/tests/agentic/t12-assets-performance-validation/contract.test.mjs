import test from 'node:test';
import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, resolve, relative, isAbsolute } from 'node:path';
import { fileURLToPath } from 'node:url';

// Oracle préalable : T03 §3/4/5/6 et qa-plan G-01..08/T-07.
// Contrôle documentaire local, pas parseur YAML V2, routeur, cooker ou verdict GPU.
// Aucun import du validateur T04, aucune commande tirée des fixtures.
const root = fileURLToPath(new URL('../../../../', import.meta.url));
const local = '.opencode/tests/agentic/t12-assets-performance-validation/';
const asset = '.opencode/skills/artist-asset-cooking/';
const perf = '.opencode/skills/artist-performance/';
const build = '.opencode/skills/artist-build-validation/';
const read = (path) => {
  assert.ok(existsSync(resolve(root, path)), `contrat documentaire absent : ${path}`);
  return readFileSync(resolve(root, path), 'utf8').replaceAll('\r\n', '\n');
};
const manifest = JSON.parse(read(`${local}manifest.json`));
const cases = JSON.parse(read(`${local}fixtures/scenarios.json`));
const contains = (path, patterns) => {
  const text = read(path);
  for (const pattern of patterns) assert.match(text, pattern, `${path} : ${pattern}`);
};
const metadata = (path) => {
  const match = /^---\n([\s\S]*?)\n---\n/.exec(read(path));
  assert.ok(match, `${path} : frontmatter attendu`);
  return match[1];
};

test('oracle explicite T12, deux agents et trois skills préservés', () => {
  assert.equal(manifest.schemaVersion, 1);
  assert.equal(manifest.lot, 'T12');
  assert.deepEqual(manifest.agents.map(({ id }) => id), ['asset-engineer', 'performance-reviewer']);
  assert.deepEqual(manifest.skills.map(({ id }) => id), ['artist-asset-cooking', 'artist-performance', 'artist-build-validation']);
  assert.deepEqual(manifest.obligations, ['G-01', 'G-02', 'G-03', 'G-04', 'G-05', 'G-06', 'G-07', 'G-08', 'T-07']);
  assert.ok(manifest.allowedPaths.every((p) => !isAbsolute(p) && !p.split('/').includes('..')));
  assert.equal(new Set(manifest.allowedPaths).size, manifest.allowedPaths.length);
});

test('permissions existantes inchangées, feuilles et aucun modèle imposé', () => {
  const denySubagent = 'permissions:\n  - action: subagent\n    resource: "*"\n    effect: deny';
  for (const agent of manifest.agents) {
    const front = metadata(agent.path);
    assert.match(front, /^mode: subagent$/m);
    assert.doesNotMatch(front, /^(model|variant|temperature):/m);
    const expected = denySubagent + (agent.id === 'performance-reviewer'
      ? '\n  - action: edit\n    resource: "*"\n    effect: deny' : '');
    assert.equal(front.slice(front.indexOf('permissions:')), expected);
  }
});

test('skills V2 name/description seulement, IDs locaux et entrées légères', () => {
  for (const skill of manifest.skills) {
    const front = metadata(skill.path);
    assert.match(front, new RegExp(`^name: ${skill.id}$`, 'm'));
    assert.match(front, /^description: .+$/m);
    assert.equal(front.split('\n').length, 2, 'pas de champ Cursor ou modèle');
    assert.ok(read(skill.path).split('\n').length < 130, 'progressive disclosure');
    contains(skill.path, [/## Déclencheurs et exclusions/, /## Préconditions/, /## Procédure/, /## Retour et handoff/]);
  }
});

test('liens requis résolus localement sans dépendre des branches voisines', () => {
  assert.equal(manifest.requiredLinks.length, 7);
  for (const { from, to } of manifest.requiredLinks) {
    const links = [...read(from).matchAll(/\]\(([^)#]+)(?:#[^)]*)?\)/g)].map((m) => m[1]);
    assert.ok(links.some((link) => resolve(dirname(resolve(root, from)), link) === resolve(root, to)), `${from} -> ${to}`);
    const target = relative(root, resolve(root, to));
    assert.ok(!target.startsWith('..') && !isAbsolute(target));
    read(to);
  }
});

test('asset-engineer : DDS ne choisit pas une API, frontières et handoff explicites', () => {
  contains('.opencode/agents/asset-engineer.md', [/DDS/, /DDS ne sélectionne pas D3D12/, /gpu-resource-engineer/, /gpu-portability-engineer/, /DEFER_TO/, /Pin observé/]);
});

test('performance-reviewer : diagnostic lecture seule puis auteur du goulot via lead', () => {
  contains('.opencode/agents/performance-reviewer.md', [/diagnostic en lecture seule/, /DEFER_TO/, /auteur du goulot/, /Ne modifie ni les sources ni les tests/, /Pin observé/]);
});

test('import IR cook runtime : schéma, unités, tailles et invalidation', () => {
  contains(`${asset}references/data-contract.md`, [/Source/, /représentation intermédiaire/, /Cook/, /Runtime/, /endianness/, /alignement/, /version/, /offset <= taille/, /longueur <= taille - offset/, /dépendances/, /clé de cache/, /round-trip/, /déterminisme/]);
});

test('identité résidence objet GPU : upload annulé, remplacement, retrait différé', () => {
  contains(`${asset}references/data-contract.md`, [/identité persistante/, /handle générationnel/, /résidence/, /objet GPU/, /annulation/, /remplacement/, /toutes les queues/, /PSO/, /driver/, /pas un type par/]);
});

test('géométrie : tangentes, indices, cache, meshlets et LOD sans promesse mesh shaders', () => {
  contains(`${asset}references/geometry.md`, [/unités/, /tangent\.w/, /cross\(N, T\)/, /index < vertexCount/, /cache/, /meshlets/, /LOD/, /erreur/, /mesh shaders/, /Nominal/, /Négatif/, /DirectXMesh/, /meshoptimizer/]);
});

test('textures : blocs/pitch/mips/colorimétrie, DDS tronqué et format inconnu', () => {
  contains(`${asset}references/textures.md`, [/DDS/, /BC1/, /ASTC/, /sRGB/, /rowPitch/, /slicePitch/, /ceil/, /DX10/, /148/, /128/, /159/, /160/, /tronqué/, /non supporté/, /D-01/, /D-02/, /D-03/, /DirectXTex/]);
  assert.deepEqual(cases.dds.mipBytes, [16, 8, 8]);
  assert.equal(cases.dds.legacyHeaderBytes + cases.dds.mipBytes.reduce((a, b) => a + b, 0), 160);
  assert.equal(cases.dds.totalBytes, 160);
  assert.deepEqual(cases.dds.skills, ['artist-asset-cooking']);
  assert.equal(cases.dds.principal, 'asset-engineer');
  assert.equal(cases.dds.notImplied, 'd3d12-backend-engineer');
});

test('perf : A/B équitable et données brutes répétées, coûts déplacés', () => {
  contains(`${perf}references/protocol.md`, [/A\/B/, /qualité/, /workload/, /options/, /warm-up/, /répétitions/, /dispersion/, /médiane/, /code-size/, /build/, /P-01/, /P-02/, /P-03/]);
  assert.equal(cases.performance.expected, 'HYPOTHESIS');
  assert.deepEqual(cases.performance.skills, ['artist-performance']);
});

test('perf : CPU submit, GPU, waits, spacing, compteurs et limites outils distincts', () => {
  contains(`${perf}references/protocol.md`, [/CPU submit/, /GPU/, /waits/, /spacing/, /compteurs/, /timestamps/, /différée/, /vsync/, /capture seule/, /run unique/, /INCONCLUSIVE/, /DEFER_TO/]);
});

test('build : Conan 2, baseline datée conservée, pas de nouveau total historique', () => {
  contains(`${build}SKILL.md`, [/Conan 2/, /CMakeDeps/, /CMakeToolchain/, /2026-10-03/, /87 tests unitaires \+ 22 OpenGL/, /--no-tests=error/]);
  assert.doesNotMatch(read(`${build}SKILL.md`), /\b(?:143|165)\b/);
});

test('validation commune : CPU offline GPU image perf, oracle et candidat', () => {
  contains(`${build}references/validation-levels.md`, [/CPU/, /offline/, /GPU/, /image/, /performance/, /HEAD/, /hashes/, /delta/, /découverts/, /exécutés/, /échoués/, /skipped/, /executed/, /static-presence/, /inference/]);
});

test('validation honnête : zéro, skip, preuve périmée et FAIL + blocage', () => {
  contains(`${build}references/validation-levels.md`, [/V-01/, /V-02/, /V-03/, /V-04/, /V-05/, /V-06/, /zéro test/, /skip requis/, /périmée/, /INCONCLUSIVE/, /FAIL/, /logiciel/, /mock/]);
  assert.deepEqual(cases.validation.map(({ expected }) => expected), ['INCONCLUSIVE', 'INCONCLUSIVE', 'INCONCLUSIVE', 'INCONCLUSIVE', 'FAIL', 'PASS']);
});

test('sources : rédaction originale, liens seulement et pas installation au chargement', () => {
  for (const path of [`${asset}references/geometry.md`, `${asset}references/textures.md`, `${perf}references/protocol.md`, `${build}references/validation-levels.md`]) {
    contains(path, [/https:\/\//, /rédaction originale/i, /licence/, /commit/]);
  }
  for (const { path } of manifest.skills) contains(path, [/installation/, /INCONCLUSIVE/]);
  assert.equal(cases.evidenceKind, 'static-presence');
});
