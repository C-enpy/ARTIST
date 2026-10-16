import { isDeepStrictEqual } from 'node:util';

// Local static checker, NOT a native OpenCode permission evaluator. The accepted
// YAML subset is deliberately narrow: ordered description/mode/permissions,
// scalar strings, rule triples, no aliases, blocks, tags, duplicate/unknown keys.
// Never import a fixture or execute profile content. Unsupported syntax fails.
function scalar(raw) {
  if (raw.startsWith('"')) {
    const value = JSON.parse(raw);
    if (typeof value !== 'string' || !value.trim()) throw new Error('scalar');
    return value;
  }
  if (!/^[a-z][a-z0-9-]*$/.test(raw) || ['true', 'false', 'null'].includes(raw)) {
    throw new Error('scalar');
  }
  return raw;
}

function frontmatter(text) {
  const match = /^---\r?\n([\s\S]*?)\r?\n---(?:\r?\n|$)/.exec(text);
  if (!match) throw new Error('frontmatter');
  const lines = match[1].split(/\r?\n/);
  if (!lines[0]?.startsWith('description: ') || !lines[1]?.startsWith('mode: ') || lines[2] !== 'permissions:') {
    throw new Error('fields');
  }
  scalar(lines[0].slice('description: '.length));
  if (scalar(lines[1].slice('mode: '.length)) !== 'subagent') throw new Error('mode');
  const permissions = [];
  if (lines.length < 6 || (lines.length - 3) % 3 !== 0) throw new Error('rules');
  for (let i = 3; i < lines.length; i += 3) {
    const values = ['  - action: ', '    resource: ', '    effect: '].map((prefix, j) => {
      if (!lines[i + j].startsWith(prefix)) throw new Error('rule shape');
      return scalar(lines[i + j].slice(prefix.length));
    });
    if (!['allow', 'ask', 'deny'].includes(values[2])) throw new Error('effect');
    permissions.push({action: values[0], resource: values[1], effect: values[2]});
  }
  return {permissions, body: text.slice(match[0].length)};
}

export function inspectProfile(text, role, oracle) {
  let parsed;
  try { parsed = frontmatter(text); } catch { return ['G-03.frontmatter']; }
  const errors = [];
  if (!isDeepStrictEqual(parsed.permissions, oracle.permissions)) errors.push('T-06.permissions');
  const method = parsed.body.indexOf('## Méthode portable');
  const pin = parsed.body.indexOf('## Pin ARTIST observé');
  if (method < 0 || pin <= method) errors.push('T-01.order');
  const prose = parsed.body.replace(/\s+/g, ' ');
  for (const clause of [...oracle.common, ...oracle.byAgent[role]]) {
    if (clause.phrases.some(phrase => !prose.includes(phrase))) errors.push(clause.id);
  }
  return errors;
}

export function inspectManifest(value, expected) {
  // Exact local oracle for the T05 slice of T03 §6, not the shared T04 parser.
  // Reject unknown keys, duplicate/missing IDs and widened ownership together.
  if (!value || typeof value !== 'object' || Array.isArray(value)) return ['manifest.type'];
  const errors = [];
  if (!isDeepStrictEqual(Object.keys(value).sort(), Object.keys(expected).sort())) errors.push('manifest.keys');
  for (const key of Object.keys(expected)) {
    if (!isDeepStrictEqual(value[key], expected[key])) errors.push(`manifest.${key}`);
  }
  return errors;
}
