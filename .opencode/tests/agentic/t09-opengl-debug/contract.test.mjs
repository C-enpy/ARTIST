import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

// Oracle éditorial T03 §§3–6 et qa-plan G-01..07/T-06/T-07, écrit avant le lot.
// Contrôles de présence statique : ni parser YAML général, ni routage natif,
// ni canari de permission, ni preuve de compilation/rendu/performance GPU.
const root = fileURLToPath(new URL('../../../../', import.meta.url));
const local = '.opencode/tests/agentic/t09-opengl-debug/';
const gpu = '.opencode/agents/gpu-engineer.md';
const verifier = '.opencode/agents/rendering-verifier.md';
const backend = '.opencode/skills/opengl-backend/SKILL.md';
const debug = '.opencode/skills/artist-gpu-debug/SKILL.md';
const context = '.opencode/skills/opengl-backend/references/context-lifetime.md';
const capture = '.opencode/skills/artist-gpu-debug/references/frame-capture.md';
const read = (relative) => {
  assert.ok(existsSync(path.join(root, relative)), `G-02 : livrable absent ${relative}`);
  return readFileSync(path.join(root, relative), 'utf8').replaceAll('\r\n', '\n');
};
const manifest = JSON.parse(read(`${local}manifest.json`));

test('G-07 : manifest explicite, propriété T09 et obligations stables', () => {
  assert.equal(manifest.schemaVersion, 1);
  assert.equal(manifest.lot, 'T09');
  assert.deepEqual(manifest.agents.map(({ id }) => id), ['gpu-engineer', 'rendering-verifier']);
  assert.deepEqual(manifest.skills.map(({ id }) => id), ['opengl-backend', 'artist-gpu-debug']);
  assert.deepEqual(manifest.obligations, ['G-01', 'G-02', 'G-03', 'G-04', 'G-05', 'G-06', 'G-07', 'T-06', 'T-07']);
  assert.deepEqual(manifest.allowedPaths, [gpu, verifier,
    '.opencode/skills/opengl-backend/**', '.opencode/skills/artist-gpu-debug/**',
    `${local}**`, '.opencode/evidence/mission-gpu-qa/t09-result.md']);
});

test('G-03 : profils feuilles V2 sans modèle imposé', () => {
  for (const { path: file, skills } of manifest.agents) {
    const text = read(file);
    assert.match(text, /^---\ndescription: .+\nmode: subagent\npermissions:\n/);
    assert.match(text, /- action: subagent\n    resource: "\*"\n    effect: deny/);
    assert.doesNotMatch(text.split('---')[1], /\b(?:model|variant|temperature|tools):/);
    for (const id of skills) assert.ok(text.includes(id), `${file} doit référencer ${id}`);
  }
});

test('T-06 : permissions du verifier préservées, diagnostic sans correctif', () => {
  const text = read(verifier);
  const permissions = text.slice(text.indexOf('permissions:'), text.indexOf('\n---', 4));
  assert.equal(permissions, 'permissions:\n  - action: subagent\n    resource: "*"\n    effect: deny\n  - action: edit\n    resource: "*"\n    effect: deny');
  for (const term of ['lecture seule', 'DEFER_TO', 'D-PERM', 'shell/MCP', 'golden', 'ne les applique pas']) {
    assert.ok(text.includes(term), `frontière RO manquante : ${term}`);
  }
});

test('G-04 : OpenGL noir et frame RenderDoc ont leurs principaux distincts', () => {
  assert.match(read(gpu), /description: .*OpenGL/);
  assert.match(read(gpu), /OpenGL noir/);
  assert.match(read(verifier), /description: .*RenderDoc/);
  assert.match(read(verifier), /frame RenderDoc/);
  assert.equal(existsSync(path.join(root, '.opencode/agents/opengl-backend-engineer.md')), false);
});

test('G-03 : metadata locale des skills et six IDs historiques conservés', () => {
  for (const { id, path: file } of manifest.skills) {
    const text = read(file);
    assert.ok(text.startsWith(`---\nname: ${id}\ndescription: `));
    assert.match(text, /^---\nname: [^\n]+\ndescription: [^\n]+\n---\n/);
    assert.ok(text.split('\n').length < 160, 'entrée courte, détails dans références');
    for (const heading of ['Déclencheurs', 'Exclusions', 'Préconditions', 'Oracle', 'Handoff']) {
      assert.ok(text.includes(heading), `${id} : ${heading} absent`);
    }
  }
  for (const id of ['artist-asset-cooking', 'artist-build-validation', 'artist-cpp-contracts',
    'artist-gpu-debug', 'artist-performance', 'artist-static-boundary']) {
    assert.ok(read(`.opencode/skills/${id}/SKILL.md`).includes(`name: ${id}\n`));
  }
});

test('G-02/G-05 : contexte et capacités OpenGL, pas de 4.x implicite en 3.3', () => {
  const text = read(backend) + read(context);
  for (const term of ['3.3 core', 'GL_VERSION', 'GL_SHADING_LANGUAGE_VERSION', 'GL_CONTEXT_PROFILE_MASK',
    'glGetStringi', 'thread', 'share group', '4.3', '4.4', 'ARB_compute_shader',
    'ARB_shader_storage_buffer_object', 'ARB_buffer_storage', 'KHR_debug', 'non garanti']) {
    assert.ok(text.includes(term), `précondition OpenGL manquante : ${term}`);
  }
});

test('G-02 : création, upload, état, draw, consommation et destruction concrets', () => {
  const text = read(context);
  for (const term of ['VAO', 'VBO', 'FBO', 'GL_ELEMENT_ARRAY_BUFFER', 'glVertexAttribIPointer',
    'GL_UNPACK_ALIGNMENT', 'GL_COMPILE_STATUS', 'GL_LINK_STATUS', 'glGetShaderInfoLog',
    'glGetProgramInfoLog', 'GL_FRAMEBUFFER_COMPLETE', 'glCheckFramebufferStatus', 'viewport',
    'scissor', 'depth', 'stencil', 'blend', 'GL_FRAMEBUFFER_SRGB', 'génération', 'destruction']) {
    assert.ok(text.includes(term), `cycle OpenGL incomplet : ${term}`);
  }
});

test('G-05 : synchronisation distingue ordre, visibilité et attente CPU', () => {
  const text = read(context);
  for (const term of ['glMemoryBarrier', 'GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT', 'glFenceSync',
    'glWaitSync', 'glClientWaitSync', 'glFlush', 'glFinish', 'visibilité', 'attente CPU',
    'consommateur', 'glDelete', 'non partageables']) {
    assert.ok(text.includes(term), `domaine ou portée absent : ${term}`);
  }
  assert.match(text, /pas une barrière universelle/);
  assert.match(text, /ne prouve pas la complétion/);
});

test('G-02/G-06 : oracles nominaux et négatifs sans mock GPU', () => {
  const text = read(backend) + read(debug) + read(context);
  for (const term of ['Nominal', 'Négatif', 'FBO incomplet', 'link échoué', 'sans contexte courant',
    'premier pass incorrect', 'seed', 'caméra', 'matrices', 'handedness', 'sRGB', 'tolérance',
    'INCONCLUSIVE', 'mock', 'performance']) {
    assert.ok(text.includes(term), `oracle/réduction absent : ${term}`);
  }
});

test('G-01/G-04 : capture conditionnelle et candidat rdc-cli sans installation', () => {
  const text = read(capture);
  for (const term of ['rdc-cli', 'candidat documentaire', 'non installé', 'fallback manuel',
    'CLI-Anything', 'event', 'draw', 'attachment', 'canal', 'replay', 'daemon',
    'aucune installation', 'aucun MCP', 'SHA d’arbre', 'commit', 'notice', 'licence',
    'https://renderdoc.org/docs/', 'https://github.com/rudybear/renderdoc-skill']) {
    assert.ok(text.includes(term), `contrat capture absent : ${term}`);
  }
});

test('T-07/G-06 : pin, candidat, comptages et niveaux de preuve qualifiés', () => {
  const text = read(backend) + read(debug) + read(capture);
  for (const term of ['HEAD', 'hash', 'delta', 'unique_ptr', 'emprunt', 'Proposed', 'partial',
    'découverts', 'exécutés', 'skipped', 'static-presence', 'executed', 'inference',
    'zéro test', 'CPU', 'GPU', 'image', 'performance']) {
    assert.ok(text.includes(term), `pin/preuve absent : ${term}`);
  }
});

test('G-03/G-04 : liens progressifs locaux résolus, pas de stub voisin', () => {
  assert.equal(manifest.requiredLinks.length, 10);
  for (const { from, to } of manifest.requiredLinks) {
    const links = [...read(from).matchAll(/\[[^\]]+\]\(([^)]+)\)/g)]
      .map((match) => path.resolve(root, path.dirname(from), match[1]));
    assert.ok(links.includes(path.resolve(root, to)), `lien requis absent : ${from} -> ${to}`);
    assert.ok(existsSync(path.join(root, to)), `cible locale absente : ${to}`);
  }
});
