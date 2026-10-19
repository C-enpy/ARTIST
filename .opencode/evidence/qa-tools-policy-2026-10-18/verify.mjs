// Runner lead : contrôles de déclaration/découverte, PAS évaluateur natif de gates.
// Oracle T05 historique conservé ; seules les trois décisions utilisateur changent
// dans l'oracle de ce lot, en mémoire. Aucun profil/import candidat exécuté.
import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import { createHash } from 'node:crypto';
import { execFileSync, spawnSync } from 'node:child_process';
import { inspectProfile } from '../../tests/agentic/t05-qa/fixtures/offline-checks.mjs';

const root = process.cwd();
const cli = process.argv[2];
assert.ok(cli && fs.existsSync(cli), 'CLI natif découvert requis en argument');
const roles = ['qa-planner', 'test-engineer', 'test-reviewer'];
const opened = ['shell', 'execute', 'subagent'];
const oldOracle = JSON.parse(fs.readFileSync('.opencode/tests/agentic/t05-qa/fixtures/expectations.json', 'utf8'));
const permissions = oldOracle.permissions.map(r => opened.includes(r.action) ? { ...r, effect: 'allow' } : r);
const run = args => execFileSync(cli, args, { cwd: root, encoding: 'utf8', timeout: 30000 });
const resolved = JSON.parse(run(['api', 'agent.list', '--param', `location[directory]=${root}`])).data;
const records = [];
for (const role of roles) {
  const file = `.opencode/agents/${role}.md`;
  const bytes = fs.readFileSync(file);
  const content = bytes.toString('utf8');
  const declarationOracle = { permissions, common: [], byAgent: { [role]: [] } };
  assert.deepEqual(inspectProfile(content, role, declarationOracle), [], `${role}: syntaxe et règles exactes`);
  const before = execFileSync('git', ['show', `230c551:${file}`], { encoding: 'utf8' });
  assert.deepEqual(inspectProfile(before, role, { ...declarationOracle, permissions: oldOracle.permissions }), []);
  const agent = resolved.find(a => a.id === role);
  assert.ok(agent, `${role}: découverte native`);
  assert.equal(agent.mode, 'subagent');
  // OpenCode 2.0.32 ajoute un refus browser APRÈS les règles du profil.
  // Ne pas supposer que les 27 règles locales constituent la fin du tableau.
  const suffix = [...permissions, { action: 'browser', resource: '*', effect: 'deny' }];
  assert.deepEqual(agent.permissions.slice(-suffix.length), suffix, `${role}: règles natives et refus browser final`);
  assert.ok(JSON.stringify(agent.system).includes('chacun garde ses propres permissions'));
  assert.equal(content.includes('execute est denied'), false);
  assert.equal(content.includes('Code Mode fermés'), false);
  records.push({ role, sha256: createHash('sha256').update(bytes).digest('hex'),
    declaredRules: permissions.length, resolvedRules: agent.permissions.length,
    status: 'PASS_DECLARATION_AND_DISCOVERY_ONLY' });
}
const diffCheck = spawnSync('git', ['diff', '--check'], { cwd: root, encoding: 'utf8' });
assert.equal(diffCheck.status, 0, diffCheck.stdout + diffCheck.stderr);
const report = {
  at: new Date().toISOString(), runner: 'lead', node: process.version,
  opencode: run(['--version']).trim(), root,
  head: execFileSync('git', ['rev-parse', 'HEAD'], { encoding: 'utf8' }).trim(),
  delta: execFileSync('git', ['diff', '--name-only'], { encoding: 'utf8' }).trim().split('\n'),
  records,
  nestedTools: 'Pas de grant MCP/team ajouté ; deny-all final applicable aux actions sans exception.',
  limits: 'Déclaration et découverte ne prouvent ni refus à invocation ni confinement filesystem.',
};
const output = path.join(root, 'out/qa-tools-policy-2026-10-18');
fs.mkdirSync(output, { recursive: true });
fs.writeFileSync(path.join(output, 'declarations.json'), JSON.stringify(report, null, 2) + '\n');
console.log(JSON.stringify(report, null, 2));
