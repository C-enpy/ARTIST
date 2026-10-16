import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';

// Contrôle possédé par le runner, jamais un chemin hérité à nettoyer.
export function gitControls(root) {
  for (const name of ['home', 'hooks', 'templates']) fs.mkdirSync(path.join(root, name), { recursive: true });
  fs.writeFileSync(path.join(root, 'empty.config'), '');
  return root;
}

export function isolatedGitEnvironment(control, inherited = process.env) {
  const env = Object.fromEntries(Object.entries(inherited).filter(([k]) =>
    !/^GIT_/i.test(k) && !/^(HOME|USERPROFILE|XDG_CONFIG_HOME)$/i.test(k)));
  return { ...env, HOME: path.join(control, 'home'), USERPROFILE: path.join(control, 'home'),
    XDG_CONFIG_HOME: path.join(control, 'home'), GIT_CONFIG_NOSYSTEM: '1',
    GIT_CONFIG_SYSTEM: path.join(control, 'empty.config'), GIT_CONFIG_GLOBAL: path.join(control, 'empty.config'),
    GIT_ATTR_NOSYSTEM: '1', GIT_OPTIONAL_LOCKS: '0',
    GIT_CONFIG_COUNT: '3', GIT_CONFIG_KEY_0: 'core.hooksPath', GIT_CONFIG_VALUE_0: path.join(control, 'hooks'),
    GIT_CONFIG_KEY_1: 'init.templateDir', GIT_CONFIG_VALUE_1: path.join(control, 'templates'),
    GIT_CONFIG_KEY_2: 'core.fsmonitor', GIT_CONFIG_VALUE_2: 'false' };
}

// Réservé aux mkdtemp possédés par les tests. Aucun nettoyage à partir d'env.
export function fixtureGit(root, inherited = process.env) {
  const canonical = fs.realpathSync(root);
  const control = gitControls(path.join(canonical, '.fixture-git'));
  const env = isolatedGitEnvironment(control, inherited);
  const invoke = args => {
    const run = spawnSync('git', ['--no-optional-locks',
      '-c', `core.hooksPath=${path.join(control, 'hooks')}`,
      '-c', `init.templateDir=${path.join(control, 'templates')}`,
      '-c', 'core.fsmonitor=false', '-c', 'core.autocrlf=false',
      '-c', `core.attributesFile=${path.join(control, 'empty.config')}`, '-C', canonical, ...args],
    { encoding: 'utf8', env, timeout: 10000, maxBuffer: 8 * 1024 * 1024 });
    assert.equal(run.error, undefined, 'Git fixture doit démarrer sans timeout');
    assert.equal(run.signal, null, 'Git fixture ne doit pas être interrompu');
    assert.equal(run.status, 0, `précondition Git fixture : ${run.stderr}`);
    return run.stdout.trim();
  };
  const identity = () => {
    const normalize = p => process.platform === 'win32' ? p.toLowerCase() : p;
    assert.equal(normalize(fs.realpathSync(invoke(['rev-parse', '--show-toplevel']))), normalize(canonical), 'racine Git fixture exacte');
    assert.equal(normalize(fs.realpathSync(invoke(['rev-parse', '--absolute-git-dir']))),
      normalize(path.join(canonical, '.git')), 'git-dir réel dans la fixture');
  };
  return args => {
    const dotgit = path.join(canonical, '.git');
    if (fs.existsSync(dotgit)) assert.ok(fs.lstatSync(dotgit).isDirectory() && !fs.lstatSync(dotgit).isSymbolicLink(),
      'fixture : .git doit être un répertoire local, jamais un gitfile/junction');
    if (args[0] === 'init') {
      const result = invoke([...args, `--template=${path.join(control, 'templates')}`]);
      identity(); return result;
    }
    identity(); // Avant add/commit et toute autre opération, pas après mutation.
    return invoke(args);
  };
}
