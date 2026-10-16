import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import os from 'node:os';
import { fileURLToPath } from 'node:url';
import { spawnSync } from 'node:child_process';
import { fixtureGit, gitControls, isolatedGitEnvironment } from '../t04-harness/git-fixture.mjs';

const here = path.dirname(fileURLToPath(import.meta.url));
const root = path.resolve(here, '../../../..');
const catalogPath = '.opencode/evidence/mission-gpu-qa/catalog-contract.json';
const manifestPath = '.opencode/tests/agentic/t13-integration/manifest.json';
const coordinatorPath = '.opencode/agents/orchestrator.md';
// Source-derived oracles, written before the integration documents. No runtime
// selector: these assertions check declarations, not observed agent behaviour.
const domainPairs = [
  ['capabilities', 'gpu-portability-engineer', 'gpu-capability-detection'],
  ['graphics-pipelines', 'gpu-pipeline-engineer', 'gpu-pipelines'],
  ['compute-pipelines', 'gpu-pipeline-engineer', 'gpu-pipelines'],
  ['shader-compilation', 'shader-engineer', 'gpu-shader-compilation'],
  ['shader-optimization', 'shader-engineer', 'artist-performance'],
  ['resources', 'gpu-resource-engineer', 'gpu-resource-lifecycle'],
  ['synchronization', 'gpu-resource-engineer', 'gpu-resource-lifecycle'],
  ['render-graph', 'render-graph-engineer', 'gpu-render-graph'],
  ['frame-debugging', 'rendering-verifier', 'artist-gpu-debug'],
  ['profiling', 'performance-reviewer', 'artist-performance'],
  ['geometry', 'asset-engineer', 'artist-asset-cooking'],
  ['textures', 'asset-engineer', 'artist-asset-cooking'],
  ['simulation', 'gpu-simulation-engineer', 'gpu-compute-simulation'],
  ['portability', 'gpu-portability-engineer', 'gpu-backend-portability'],
  ['validation', 'rendering-verifier', 'artist-build-validation'],
  ['vulkan', 'vulkan-backend-engineer', 'vulkan-backend'],
  ['d3d12', 'd3d12-backend-engineer', 'd3d12-backend'],
  ['metal', 'metal-backend-engineer', 'metal-backend'],
  ['opengl', 'gpu-engineer', 'opengl-backend'],
];
const selections = [
  ['S01', 'gpu-pipeline-engineer', ['gpu-pipelines', 'vulkan-backend']],
  ['S02', 'shader-engineer', ['artist-performance', 'gpu-shader-compilation']],
  ['S03', 'gpu-resource-engineer', ['gpu-resource-lifecycle', 'd3d12-backend']],
  ['S04', 'shader-engineer', ['gpu-shader-compilation', 'gpu-backend-portability']],
  ['S05', 'gpu-simulation-engineer', ['gpu-compute-simulation', 'gpu-pipelines']],
  ['S06', 'rendering-verifier', ['artist-gpu-debug']],
  ['S07', 'asset-engineer', ['artist-asset-cooking']],
  ['S08', 'render-graph-engineer', ['gpu-render-graph', 'artist-static-boundary']],
  ['S09', 'gpu-engineer', ['artist-gpu-debug', 'opengl-backend']],
  ['S10', 'performance-reviewer', ['artist-performance']],
  ['S11', 'gpu-portability-engineer', ['gpu-capability-detection']],
  ['S12', 'gpu-portability-engineer', ['gpu-backend-portability', 'artist-gpu-debug']],
];
const producerLots = ['t05-qa', 't06-vulkan', 't07-d3d12', 't08-metal',
  't09-opengl-debug', 't10-pipelines-shaders-simulation',
  't11-resources-graph-portability', 't12-assets-performance-validation'];

test('manual catalog v1: scope, source pins, no native claims', () => {
  const c = JSON.parse(fs.readFileSync(path.join(root, catalogPath), 'utf8'));
  assert.equal(c.schemaVersion, 1);
  assert.equal(c.kind, 'static-test-oracle');
  assert.equal(c.base, '1821af67ce75f838d2ae35349a1085efa8d3168b');
  assert.equal(c.operatorBase, '82edafee15e42cea142b9b807598420cac84ee06');
  assert.deepEqual(c.apis, ['OpenGL', 'Vulkan', 'D3D12', 'Metal']);
  assert.deepEqual(c.implementedBackends, ['OpenGL']);
  assert.equal(c.nativeStatus, 'NOT_RUN');
  assert.equal(c.domains.length, 19);
  assert.equal(c.scenarios.length, 12);
  assert.deepEqual(c.qaLoop, {
    triggers: ['contract-behaviour', 'extended-integration-suite', 'pm-risk-at-least-50'],
    pmRiskThreshold: 50, qaImpactRange: [1, 5], qaProbabilityRange: [1, 5],
    qaFiveFieldsThreshold: 10,
    order: ['planner', 'lead-commits-plan', 'authors-tdd', 'integrate-producers', 'generator-gaps', 'fresh-reviewer-off-board', 're-audit'],
    runner: 'separate-trusted-lead', autonomousEditing: false, nativeEnforcement: 'INCONCLUSIVE',
  });
  for (const source of c.sources) {
    assert.ok(fs.existsSync(path.join(root, source)), source);
  }
});

for (const [id, principal, skill] of domainPairs) {
  test(`domain ${id}: explicit nominal/negative oracle and coordinator entry`, () => {
    const c = JSON.parse(fs.readFileSync(path.join(root, catalogPath), 'utf8'));
    const rows = c.domains.filter(d => d.id === id);
    assert.equal(rows.length, 1);
    assert.equal(rows[0].principal, principal);
    assert.equal(rows[0].skill, skill);
    for (const key of ['nominal', 'negative', 'oracle']) assert.ok(rows[0][key]?.length > 15, key);
    const text = fs.readFileSync(path.join(root, coordinatorPath), 'utf8');
    assert.ok(text.includes(`\`${id}\` → \`${principal}\` / \`${skill}\``));
  });
}

for (const [id, principal, skills] of selections) {
  test(`${id}: selection/minimum skills are expectations, not native observations`, () => {
    const c = JSON.parse(fs.readFileSync(path.join(root, catalogPath), 'utf8'));
    const rows = c.scenarios.filter(s => s.id === id);
    assert.equal(rows.length, 1);
    assert.equal(rows[0].principal, principal);
    assert.deepEqual(rows[0].minimumSkills, skills);
    assert.equal(rows[0].status, 'NOT_RUN');
    assert.ok(rows[0].prompt.length > 10);
    assert.ok(rows[0].negative.length > 15);
    const text = fs.readFileSync(path.join(root, coordinatorPath), 'utf8');
    const line = text.split('\n').find(l => l.startsWith(`${id} :`));
    assert.ok(line?.includes(`\`${principal}\``));
    for (const skill of skills) assert.ok(line.includes(`\`${skill}\``));
  });
}

const clauses = [
  'description → skill pertinent → référence ciblée',
  'comportement sous contrat OU suite d’intégration étendue OU risque PM >= 50',
  'plan QA commité avant les worktrees auteurs',
  'intégration des producteurs avant le generator',
  'test-reviewer fresh hors board puis ré-audit',
  'Le generator complète les gaps, jamais le RED des auteurs',
  'QA = impact × probabilité (1–5 chacun, produit 1–25)',
  'QA >= 10 : déclencheur, blast radius, détection, mitigation, gap',
  'Runner séparé : propositions texte/patch',
  'le lead persiste, relit et exécute',
  'execute fermé rend les MCP uniquement Code Mode indisponibles',
  'template déclaré reconnu ne prouve ni sandbox ni enforcement',
  'CPU seul → `cpp-engineer`',
  'DDS seul ne sélectionne pas D3D12',
  'nom RTX insuffisant',
  'lifetime et hazards avant optimisation',
];
for (const clause of clauses) {
  test(`coordinator clause: ${clause}`, () => {
    const text = fs.readFileSync(path.join(root, coordinatorPath), 'utf8');
    assert.ok(text.includes(clause), `missing declaration: ${clause}`);
    // A deletion must be distinguishable; this is a static text oracle only.
    assert.equal(text.replaceAll(clause, 'REMOVED').includes(clause), false);
  });
}

for (const file of ['AGENTS.md', '.opencode/commands/engine-mission.md', '.opencode/commands/engine-tdd.md']) {
  test(`${file}: QA gate and runner limitation on existing coordinator`, () => {
    const text = fs.readFileSync(path.join(root, file), 'utf8');
    for (const clause of ['comportement sous contrat', 'suite d’intégration étendue', 'risque PM >= 50',
      'qa-planner', 'test-engineer', 'test-reviewer', 'Runner séparé', '.opencode/agents/orchestrator.md']) {
      assert.ok(text.includes(clause), clause);
    }
  });
}

test('Conan 2 correction follows inspected build, not a toolchain migration', () => {
  const text = fs.readFileSync(path.join(root, '.opencode/agents/build-engineer.md'), 'utf8');
  for (const token of ['Conan 2', 'CMakeDeps', 'CMakeToolchain', 'avant CMake']) assert.ok(text.includes(token));
  assert.ok(!text.includes('Conan 1 existante'));
  const conan = fs.readFileSync(path.join(root, 'conanfile.txt'), 'utf8');
  assert.ok(conan.includes('CMakeDeps') && conan.includes('CMakeToolchain'));
});

test('union manifest covers authored contracts without granting T13 their ownership', () => {
  const m = JSON.parse(fs.readFileSync(path.join(root, manifestPath), 'utf8'));
  assert.equal(m.schemaVersion, 1);
  assert.equal(m.lot, 'T13-catalog-union');
  assert.equal(m.agents.length, 23);
  assert.equal(m.skills.length, 17);
  assert.equal(new Set(m.agents.map(a => a.id)).size, 23);
  assert.equal(new Set(m.skills.map(a => a.id)).size, 17);
  for (const lot of producerLots) {
    const p = JSON.parse(fs.readFileSync(path.join(root, '.opencode/tests/agentic', lot, 'manifest.json'), 'utf8'));
    for (const a of p.agents) assert.deepEqual(m.agents.find(x => x.id === a.id), a);
    for (const s of p.skills) assert.ok(m.skills.some(x => x.id === s.id && x.path === s.path));
    for (const l of p.requiredLinks) assert.ok(m.requiredLinks.some(x => x.from === l.from && x.to === l.to));
    for (const allowed of p.allowedPaths) assert.ok(m.allowedPaths.includes(allowed), allowed);
  }
  assert.ok(!m.allowedPaths.includes('.opencode/**'));
  const ids = fs.readdirSync(path.join(root, '.opencode/agents')).map(f => f.replace(/\.md$/, '')).sort();
  assert.deepEqual(m.agents.map(a => a.id).sort(), ids);
});

test('documentation links, provenance and native plan remain explicitly qualified', () => {
  for (const file of ['docs/index.md', 'docs/opencode-harness.md']) {
    assert.ok(fs.readFileSync(path.join(root, file), 'utf8').includes('[[agentic/gpu-qa-harness|'));
  }
  const doc = fs.readFileSync(path.join(root, 'docs/agentic/gpu-qa-harness.md'), 'utf8');
  for (const term of ['t02-external-sources.md', 't02-source-inventory.md', 'INCONCLUSIVE', 'Runner séparé',
    'OpenGL', '23 profils', '17 skills', '19 responsabilités', 'QMD', 'canonique']) assert.ok(doc.includes(term), term);
  const plan = fs.readFileSync(path.join(root, '.opencode/evidence/mission-gpu-qa/native-probe-plan.md'), 'utf8');
  for (const [id] of selections) assert.ok(plan.includes(id));
  for (const term of ['NON EXÉCUTÉS', 'sentinelle', 'Code Mode', 'positif', 'négatif', 'OP1', 'OP2', 'OP3', 'NOT_RUN']) assert.ok(plan.includes(term), term);
});

// All validator mutations are confined to fresh owned directories. Never run
// git add/commit against the candidate, another worktree or an inherited GIT_DIR.
// The existing T04 helper is reused unchanged; Node/Git on PATH are trusted inputs.
const variants = [
  ['nominal', null, null, 0],
  ['missing-skill', 'skill', 'SKILL_MISSING', 1],
  ['collision', 'collision', 'ID_COLLISION', 1],
  ['missing-link', 'link', 'FILE_MISSING', 1],
  ['escape', 'escape', 'PATH_UNSAFE', 1],
  ['malformed-coordinator', 'syntax', 'YAML_MALFORMED', 1],
  ['permission-widened', 'permission', 'PERMISSION_QA', 1],
  ['native-unavailable', 'native', null, 2],
  ['native-unavailable-plus-fail', 'native-fail', 'FILE_MISSING', 1],
];
for (const [name, mutation, code, exit] of variants) {
  test(`T04 CLI cross-validation: ${name}`, () => {
    // Short owned path: deeply nested Ensemble worktrees exceed Git/Win32 MAX_PATH.
    const temporaryParent = process.platform === 'win32' ? path.join(os.tmpdir(), 'opencode') : os.tmpdir();
    const tmp = fs.mkdtempSync(path.join(temporaryParent, 't13-'));
    try {
      for (const dir of ['.opencode/agents', '.opencode/skills', 'docs']) {
        fs.cpSync(path.join(root, dir), path.join(tmp, dir), { recursive: true });
      }
      const m = JSON.parse(fs.readFileSync(path.join(root, manifestPath), 'utf8'));
      for (const { from, to } of m.requiredLinks) for (const file of [from, to]) {
        fs.mkdirSync(path.dirname(path.join(tmp, file)), { recursive: true });
        fs.copyFileSync(path.join(root, file), path.join(tmp, file));
      }
      if (mutation === 'skill') m.agents[0].skills.push('nonexistent-skill');
      if (mutation === 'collision') m.agents.push(m.agents[0]);
      if (mutation === 'link' || mutation === 'native-fail') m.requiredLinks.push({ from: coordinatorPath, to: 'absent.md' });
      if (mutation === 'escape') m.allowedPaths.push('../escape');
      if (mutation === 'syntax') {
        const p = path.join(tmp, coordinatorPath);
        fs.writeFileSync(p, fs.readFileSync(p, 'utf8').replace(/^---/, 'BROKEN'));
      }
      if (mutation === 'permission') {
        const p = path.join(tmp, '.opencode/agents/qa-planner.md');
        fs.writeFileSync(p, fs.readFileSync(p, 'utf8').replace('effect: deny', 'effect: allow'));
      }
      fs.writeFileSync(path.join(tmp, 'manifest.json'), JSON.stringify(m));
      const git = fixtureGit(tmp);
      git(['init']); git(['add', '.']);
      git(['-c', 'user.name=T13 fixture', '-c', 'user.email=fixture@example.invalid', 'commit', '-qm', 'owned fixture']);
      const args = [path.join(root, '.opencode/scripts/validate-agentic.mjs'), '--mode',
        mutation?.startsWith('native') ? 'native-evidence' : 'offline', '--root', tmp, '--manifest', 'manifest.json'];
      if (mutation?.startsWith('native')) {
        // Deliberately empty unit-test input, NOT an observation packet.
        fs.writeFileSync(path.join(tmp, 'empty-input.json'), '{"schemaVersion":1}');
        args.push('--evidence', 'empty-input.json');
      }
      const control = gitControls(path.join(tmp, '.runner'));
      const run = spawnSync(process.execPath, args, { cwd: tmp, encoding: 'utf8',
        env: isolatedGitEnvironment(control), timeout: 30000, maxBuffer: 8 * 1024 * 1024 });
      assert.equal(run.error, undefined);
      assert.equal(run.signal, null);
      const result = JSON.parse(run.stdout);
      assert.equal(run.status, exit, JSON.stringify(result.checks.filter(c => c.verdict !== 'PASS')));
      assert.equal(result.missionVerdict, 'INCONCLUSIVE');
      if (code) assert.ok(result.checks.some(c => c.code === code && c.verdict === 'FAIL'), code);
      if (mutation === null) {
        assert.equal(result.verdict, 'PASS');
        const qa = result.checks.filter(c => c.code === 'PERMISSION_QA' && c.observed.form === 'B');
        assert.equal(qa.length, 3);
        assert.ok(qa.every(c => c.evidenceKind === 'static-presence'));
      }
    } finally { fs.rmSync(tmp, { recursive: true, force: true }); }
  });
}
