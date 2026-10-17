import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync, existsSync } from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { inspectProfile, inspectManifest } from './fixtures/offline-checks.mjs';

const here = path.dirname(fileURLToPath(import.meta.url));
const root = path.resolve(here, '../../../..');
const readJson = name => JSON.parse(readFileSync(path.join(here, name), 'utf8'));
const oracle = readJson('fixtures/expectations.json');
const roles = ['qa-planner', 'test-engineer', 'test-reviewer'];
const expectedPaths = [
  '.opencode/agents/qa-planner.md',
  '.opencode/agents/test-engineer.md',
  '.opencode/agents/test-reviewer.md',
  '.opencode/tests/agentic/t05-qa/contract.test.mjs',
  '.opencode/tests/agentic/t05-qa/manifest.json',
  '.opencode/tests/agentic/t05-qa/fixtures/expectations.json',
  '.opencode/tests/agentic/t05-qa/fixtures/offline-checks.mjs',
  '.opencode/tests/agentic/t05-qa/fixtures/native-canaries.json',
  '.opencode/evidence/mission-gpu-qa/t05-result.md',
];

// Synthetic document built from an explicit source-derived oracle, never the
// candidate. It exercises the checker only; real profiles are checked below.
function specimen(role, permissions = oracle.permissions) {
  const rules = permissions.flatMap(r => [
    `  - action: ${JSON.stringify(r.action)}`,
    `    resource: ${JSON.stringify(r.resource)}`,
    `    effect: ${r.effect}`,
  ]);
  const clauses = [...oracle.common, ...oracle.byAgent[role]];
  return ['---', 'description: "Offline synthetic QA profile"', 'mode: subagent',
    'permissions:', ...rules, '---', ...clauses.flatMap(c => c.phrases)].join('\n');
}

function manifestSpecimen() {
  return {
    schemaVersion: 1, lot: 't05-qa',
    agents: roles.map(id => ({id, path: `.opencode/agents/${id}.md`, skills: ['artist-build-validation']})),
    skills: [],
    obligations: ['G-03', 'G-06', 'G-07', 'T-01', 'T-03', 'T-04', 'T-05', 'T-06', 'T-07'],
    requiredLinks: roles.map(id => ({from: `.opencode/agents/${id}.md`, to: '.opencode/skills/artist-build-validation/SKILL.md'})),
    allowedPaths: expectedPaths,
  };
}

for (const role of roles) {
  test(`offline validator: ${role} nominal subset`, () => {
    assert.deepEqual(inspectProfile(specimen(role), role, oracle), []);
  });
  test(`offline validator: ${role} accepts ordinary Markdown line wrapping`, () => {
    const wrapped = specimen(role).replace('grant exact natif vérifié', 'grant exact natif\n  vérifié');
    assert.deepEqual(inspectProfile(wrapped, role, oracle), []);
  });
  for (const clause of [...oracle.common, ...oracle.byAgent[role]]) {
    test(`offline validator: ${role} rejects missing ${clause.id}`, () => {
      for (const phrase of clause.phrases) {
        const bad = specimen(role).replaceAll(phrase, 'REMOVED_CLAUSE');
        assert.ok(inspectProfile(bad, role, oracle).includes(clause.id), `missing ${phrase}`);
      }
    });
  }
  test(`offline validator: ${role} rejects all permission mutations`, () => {
    for (let i = 0; i < oracle.permissions.length; i++) {
      const changed = structuredClone(oracle.permissions);
      changed[i].effect = changed[i].effect === 'deny' ? 'allow' : 'deny';
      assert.ok(inspectProfile(specimen(role, changed), role, oracle).includes('T-06.permissions'), `rule ${i}`);
    }
    for (const action of ['shell', 'execute', 'edit', 'subagent', 'team_*', 'serena_*', 'context_mode_*']) {
      const widened = [...oracle.permissions, {action, resource: '*', effect: 'allow'}];
      assert.ok(inspectProfile(specimen(role, widened), role, oracle).includes('T-06.permissions'), action);
    }
    const wrongOrder = [...oracle.permissions.slice(1), oracle.permissions[0]];
    assert.ok(inspectProfile(specimen(role, wrongOrder), role, oracle).includes('T-06.permissions'));
  });
  test(`offline validator: ${role} rejects malformed/unsupported frontmatter`, () => {
    const good = specimen(role);
    const mutations = [
      good.replace('mode: subagent', 'mode: primary'),
      good.replace('mode: subagent', 'mode: subagent\nmode: subagent'),
      good.replace('mode: subagent', 'mode: subagent\nmodel: "fixed/model"'),
      good.replace('mode: subagent', 'mode: subagent\npermissions: []'),
      good.replace('description: "Offline synthetic QA profile"', 'description: |\n  unsupported block'),
      good.replace('description: "Offline synthetic QA profile"', 'description: false'),
      good.replace('permissions:', 'permission:'),
      good.slice(4),
    ];
    for (const bad of mutations) assert.ok(inspectProfile(bad, role, oracle).includes('G-03.frontmatter'));
  });
  test(`offline validator: ${role} rejects reversed portable/pin layers`, () => {
    const bad = specimen(role).replace('## Méthode portable', 'PIN_FIRST')
      .replace('## Pin ARTIST observé', '## Méthode portable')
      .replace('PIN_FIRST', '## Pin ARTIST observé');
    assert.ok(inspectProfile(bad, role, oracle).includes('T-01.order'));
  });
  test(`candidate ${role}: static contract (not native discovery)`, () => {
    const content = readFileSync(path.join(root, `.opencode/agents/${role}.md`), 'utf8');
    assert.deepEqual(inspectProfile(content, role, oracle), []);
  });
}

test('offline validator: manifest nominal and negative cases', () => {
  const good = manifestSpecimen();
  assert.deepEqual(inspectManifest(good, manifestSpecimen()), []);
  const mutations = [
    {...good, schemaVersion: 2}, {...good, lot: 'other-lot'}, {...good, agents: []},
    {...good, skills: [{id: 'imaginary', path: '../escape'}]},
    {...good, obligations: good.obligations.slice(1)}, {...good, obligations: [...good.obligations, 'T-99']},
    {...good, requiredLinks: []}, {...good, allowedPaths: ['.opencode/**']},
    {...good, command: 'do-not-execute'},
    {...good, allowedPaths: [...good.allowedPaths, '../outside.md']},
  ];
  for (const bad of mutations) assert.ok(inspectManifest(bad, manifestSpecimen()).length > 0);
});

test('candidate manifest: explicit schemaVersion 1, exact ownership and real links', () => {
  const candidate = readJson('manifest.json');
  assert.deepEqual(inspectManifest(candidate, manifestSpecimen()), []);
  for (const link of candidate.requiredLinks) {
    assert.ok(existsSync(path.join(root, link.from)), link.from);
    assert.ok(existsSync(path.join(root, link.to)), link.to);
  }
  for (const agent of candidate.agents) assert.equal(path.basename(agent.path, '.md'), agent.id);
});

test('canary format: proposed-only, no fabricated runtime or grant', () => {
  const packet = readJson('fixtures/native-canaries.json');
  assert.equal(packet.schemaVersion, 1);
  assert.equal(packet.status, 'INCONCLUSIVE');
  assert.equal(packet.kind, 'proposal-not-observation');
  assert.deepEqual(packet.profiles, roles);
  assert.equal(packet.nativeGrant, null);
  assert.equal(packet.observed, null);
  assert.equal(packet.runner, 'lead-distinct-from-restricted-profile');
  assert.equal(packet.disposableOnly, true);
  assert.ok(packet.requiredEvidence.includes('sentinelBeforeAfter'));
  assert.ok(packet.requiredEvidence.includes('effectiveRules'));
  assert.ok(packet.requiredEvidence.includes('candidateHEADHashesDelta'));
  assert.equal(new Set(packet.cases.map(c => c.id)).size, packet.cases.length);
  const expectedCases = [
    'read-normal', 'read-env', 'read-env-suffix', 'read-env-case', 'external-directory',
    'edit-named-artifact-default', 'edit-other-lot', 'mixed-patch', 'traversal',
    'absolute-path', 'windows-case-slashes', 'junction', 'shell-composed',
    'code-mode', 'mcp-mutator', 'subagent', 'report-transport', 'grant-exact-if-verified',
  ];
  assert.deepEqual(packet.cases.map(c => c.id), expectedCases);
  for (const c of packet.cases) {
    assert.equal(c.observed, null);
    assert.ok(['allow', 'deny', 'unproven'].includes(c.expectedGate));
    assert.equal(c.state, 'NOT_RUN');
  }
  assert.ok(packet.cases.find(c => c.id === 'grant-exact-if-verified').precondition.includes('lead'));
  assert.equal(packet.cases.find(c => c.id === 'mcp-mutator').action, null);
});
