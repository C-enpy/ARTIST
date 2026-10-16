import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

// Oracle écrit avant les profils : qa-plan G-01..G-07 et T-07 ;
// t03-contract §3.16, §4 T06, §5.1, §6. Contrôles de présence statique,
// pas un parseur YAML général, un routeur, ni une validation Vulkan.
const root = fileURLToPath(new URL('../../../../', import.meta.url));
const agentPath = '.opencode/agents/vulkan-backend-engineer.md';
const skillPath = '.opencode/skills/vulkan-backend/SKILL.md';
const recipesPath = '.opencode/skills/vulkan-backend/references/recipes.md';
const sourcesPath = '.opencode/skills/vulkan-backend/references/sources.md';
const manifestPath = '.opencode/tests/agentic/t06-vulkan/manifest.json';
const expectedPaths = [
  agentPath, skillPath, recipesPath, sourcesPath,
  '.opencode/tests/agentic/t06-vulkan/contract.test.mjs', manifestPath,
  '.opencode/evidence/mission-gpu-qa/t06-result.md',
];

function read(relativePath) {
  const filename = path.join(root, relativePath);
  assert.ok(existsSync(filename), `artefact T06 obligatoire absent : ${relativePath}`);
  return readFileSync(filename, 'utf8').replaceAll('\r\n', '\n');
}

function contains(text, terms) {
  for (const term of terms) assert.ok(text.includes(term), `clause obligatoire absente : ${term}`);
}

function frontmatter(text) {
  const match = /^---\n([\s\S]*?)\n---\n/.exec(text);
  assert.ok(match, 'frontmatter délimité obligatoire');
  return match[1];
}

function assertAgent(text) {
  const fm = frontmatter(text);
  assert.match(fm, /^description: .+Vulkan.+$/m);
  assert.match(fm, /^mode: subagent$/m);
  assert.equal(fm.slice(fm.indexOf('permissions:\n')),
    'permissions:\n  - action: subagent\n    resource: "*"\n    effect: deny');
  assert.doesNotMatch(fm, /^(model|variant|permission|tools):/m);
  contains(text, ['vulkan-backend', 'DEFER_TO', 'agent feuille', 'OpenGL']);
}

test('G-07/T-07 : manifest explicite limité au lot, sans découverte ni commande', () => {
  const manifest = JSON.parse(read(manifestPath));
  assert.deepEqual(Object.keys(manifest).sort(), [
    'schemaVersion', 'lot', 'agents', 'skills', 'obligations', 'requiredLinks', 'allowedPaths',
  ].sort());
  assert.equal(manifest.schemaVersion, 1);
  assert.equal(manifest.lot, 'T06');
  assert.deepEqual(manifest.agents, [{
    id: 'vulkan-backend-engineer', path: agentPath,
    skills: ['vulkan-backend', 'artist-build-validation'],
  }]);
  assert.deepEqual(manifest.skills, [{ id: 'vulkan-backend', path: skillPath }]);
  assert.deepEqual(manifest.obligations, ['G-01', 'G-02', 'G-03', 'G-04', 'G-05', 'G-06', 'G-07', 'T-07']);
  assert.deepEqual(manifest.allowedPaths, expectedPaths);
  assert.deepEqual(manifest.requiredLinks, [
    { from: agentPath, to: skillPath },
    { from: skillPath, to: '.opencode/skills/artist-build-validation/SKILL.md' },
    { from: skillPath, to: recipesPath },
    { from: skillPath, to: sourcesPath },
  ]);
});

test('G-03 : profil V2 feuille, sans modèle imposé ; skill annoncé sans autoinvoke désactivé', () => {
  assertAgent(read(agentPath));
  const fm = frontmatter(read(skillPath));
  assert.match(fm, /^name: vulkan-backend$/m);
  assert.match(fm, /^description: .+Vulkan.+$/m);
  assert.doesNotMatch(fm, /disable-model-invocation|autoinvoke|^model:|^variant:/m);
  assert.deepEqual(fm.split('\n').map(line => line.split(':')[0]), ['name', 'description']);
});

test('G-02/G-04 : frontière API, pin ownership et scénario 1 sans routeur concurrent', () => {
  const text = read(skillPath);
  contains(text, [
    '## Déclencheurs et exclusions', '## Pin ARTIST', 'OpenGL',
    'unique_ptr', 'getPass', 'emprunt', 'Proposed', 'partial', 'Windows Debug',
    'gpu-pipeline-engineer', 'gpu-pipelines', 'depth testing', 'principal',
    'gpu-resource-engineer', 'render-graph-engineer', 'shader-engineer', 'DEFER_TO',
    'compile', 'link', 'cook', 'runtime', 'par lot',
  ]);
  assert.doesNotMatch(text, /shared_ptr/);
});

test('G-05 : préflight conditionnel, feature supportée distincte de feature activée', () => {
  contains(read(skillPath) + read(recipesPath), [
    'vkGetPhysicalDeviceFeatures2', 'vkGetPhysicalDeviceQueueFamilyProperties',
    'vkEnumerateDeviceExtensionProperties', 'vkCreateDevice', 'pNext',
    'synchronization2', 'dynamicRendering', 'timelineSemaphore',
    'VK_LAYER_KHRONOS_validation', 'vkEnumerateInstanceLayerProperties',
    'VK_EXT_debug_utils', 'GPU-assisted', 'INCONCLUSIVE', 'vulkaninfo',
    'pas de SDK', 'compilation', 'offline', 'image', 'performance',
  ]);
});

test('G-02/G-05 : domaines de synchronisation et lifetime décrits sans resource states universels', () => {
  contains(read(skillPath) + read(recipesPath), [
    'RAW', 'WAR', 'WAW', 'ordre', 'visibilité', 'attente CPU',
    'VkBufferMemoryBarrier2', 'srcStageMask', 'dstStageMask',
    'srcAccessMask', 'dstAccessMask', 'VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT',
    'VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT', 'VK_ACCESS_2_TRANSFER_READ_BIT',
    'VK_PIPELINE_STAGE_2_HOST_BIT', 'VK_ACCESS_2_HOST_READ_BIT',
    'nonCoherentAtomSize', 'vkInvalidateMappedMemoryRanges',
    'minStorageBufferOffsetAlignment', 'memoryTypeBits',
    'VK_SHARING_MODE_EXCLUSIVE', 'release', 'acquire', 'VK_QUEUE_FAMILY_IGNORED',
    'toutes les queues', 'génération', 'vkDeviceWaitIdle', 'resource states',
  ]);
});

test('G-02/G-05/G-06 : quatre recettes nominales/négatives avec oracles API distincts', () => {
  const text = read(recipesPath);
  for (const id of ['VK-01', 'VK-02', 'VK-03', 'VK-04']) {
    const section = text.split(`## ${id} — `)[1]?.split('\n## ')[0];
    assert.ok(section, `recette ${id} absente`);
    contains(section, ['Préconditions', 'Nominal', 'Négatif', 'Oracle', 'Limite']);
  }
  contains(text, [
    'VK_FORMAT_R8G8B8A8_UNORM', 'VK_FORMAT_D32_SFLOAT', 'VkPipelineRenderingCreateInfo',
    'VK_COMPARE_OP_LESS', 'VkRenderingInfo', '0,25', '0,75', 'sRGB',
    'VK_DESCRIPTOR_TYPE_STORAGE_BUFFER', 'local_size_x = 64', '256', '1024',
    'i ^ 0xA5u', 'VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL', 'vkCmdCopyImageToBuffer',
    'VK_PIPELINE_STAGE_2_NONE', 'VK_ACCESS_2_NONE', 'VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT',
  ]);
});

test('G-01 : sources officielles ciblées, rédaction originale, aucune licence de copie présumée', () => {
  contains(read(sourcesPath), [
    'Rédaction originale', 'lien seulement', 'arbre SHA', 'commit', 'CppStudio',
    'CC-BY-4.0', 'licence', '2026-10-16',
    'https://docs.vulkan.org/spec/latest/chapters/synchronization.html',
    'https://docs.vulkan.org/refpages/latest/refpages/source/VkPipelineRenderingCreateInfo.html',
    'https://github.com/KhronosGroup/Vulkan-Guide/blob/main/chapters/enabling_features.adoc',
    'https://github.com/KhronosGroup/Vulkan-ValidationLayers',
    'https://opencode.ai/v2/docs/agents', 'https://opencode.ai/v2/docs/skills',
  ]);
});

test('G-03 : les liens déclarés existent localement et sont réellement présents dans le Markdown', () => {
  const { requiredLinks } = JSON.parse(read(manifestPath));
  for (const { from, to } of requiredLinks) {
    const links = [...read(from).matchAll(/\]\(([^)]+)\)/g)].map(match => match[1]);
    const destinations = links.filter(href => !href.includes('://')).map(href =>
      path.posix.normalize(path.posix.join(path.posix.dirname(from), href.split('#')[0])));
    assert.ok(destinations.includes(to), `lien Markdown manquant : ${from} -> ${to}`);
    assert.ok(existsSync(path.join(root, to)), `cible locale manquante : ${to}`);
  }
});

test('G-03/G-05 : mutations en mémoire détectées, aucune modification des fichiers', () => {
  const agent = read(agentPath);
  assertAgent(agent);
  assert.throws(() => assertAgent(agent.replace('effect: deny', 'effect: allow')), /AssertionError/);
  assert.throws(() => assertAgent(agent.replace('effect: deny',
    'effect: deny\n  - action: subagent\n    resource: "*"\n    effect: allow')), /AssertionError/);
  assert.throws(() => assertAgent(agent.replace('mode: subagent', 'mode: primary')), /AssertionError/);
  assert.throws(() => assertAgent(agent.replace('mode: subagent', 'mode: subagent\nmodel: forbidden')), /AssertionError/);
  const recipe = read(recipesPath);
  const required = ['VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT', 'vkInvalidateMappedMemoryRanges'];
  contains(recipe, required);
  for (const term of required) {
    assert.throws(() => contains(recipe.replaceAll(term, ''), required), /clause obligatoire absente/);
  }
});
