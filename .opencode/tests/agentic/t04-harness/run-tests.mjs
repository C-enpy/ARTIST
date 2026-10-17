import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { spawnSync } from 'node:child_process';
import { gitControls, isolatedGitEnvironment } from './git-fixture.mjs';

const directory = path.dirname(fileURLToPath(import.meta.url));
const [suite = 'all', pattern] = process.argv.slice(2);
if (!['all', 'isolation', 'contract'].includes(suite) || process.argv.length > 4) {
  throw new Error('Usage: node run-tests.mjs [all|isolation|contract] [test-name-pattern]');
}
const control = fs.mkdtempSync(path.join(directory, '.run-control-'));
try {
  gitControls(control);
  const files = (suite === 'all' ? ['isolation', 'contract'] : [suite]).map(s => path.join(directory, `${s}.test.mjs`));
  const args = ['--test', '--test-concurrency=1', '--test-reporter=tap',
    ...(pattern ? [`--test-name-pattern=${pattern}`] : []), ...files];
  console.log(JSON.stringify({ command: [process.execPath, ...args], node: process.version,
    platform: process.platform, architecture: process.arch, control }));
  const run = spawnSync(process.execPath, args, { env: isolatedGitEnvironment(control),
    encoding: 'utf8', timeout: 180000, maxBuffer: 16 * 1024 * 1024 });
  process.stdout.write(run.stdout ?? ''); process.stderr.write(run.stderr ?? '');
  if (run.error) throw run.error;
  if (run.signal) throw new Error(`Runner signal: ${run.signal}`);
  process.exitCode = run.status;
} finally { fs.rmSync(control, { recursive: true, force: true }); }
