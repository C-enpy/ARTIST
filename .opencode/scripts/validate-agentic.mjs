import fs from 'node:fs';
import path from 'node:path';
import { createHash } from 'node:crypto';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';

// Contrat LOCAL v1 : .opencode/tests/agentic/t04-harness/README.md.
// Ce programme ne charge pas OpenCode, ne lance pas de test/commande citée,
// n'accorde aucune permission et ne peut authentifier une capture native.
const MODES = ['syntax', 'schema', 'offline', 'native-evidence'];
const BASELINE = ['artist-asset-cooking', 'artist-build-validation', 'artist-cpp-contracts',
  'artist-gpu-debug', 'artist-performance', 'artist-static-boundary'];
const ROUTES = [
  ['gpu-pipeline-engineer', ['gpu-pipelines', 'vulkan-backend']],
  ['shader-engineer', ['artist-performance', 'gpu-shader-compilation']],
  ['gpu-resource-engineer', ['gpu-resource-lifecycle', 'd3d12-backend']],
  ['shader-engineer', ['gpu-shader-compilation', 'gpu-backend-portability']],
  ['gpu-simulation-engineer', ['gpu-compute-simulation', 'gpu-pipelines']],
  ['rendering-verifier', ['artist-gpu-debug']],
  ['asset-engineer', ['artist-asset-cooking']],
  ['render-graph-engineer', ['gpu-render-graph', 'artist-static-boundary']],
  ['gpu-engineer', ['artist-gpu-debug', 'opengl-backend']],
  ['performance-reviewer', ['artist-performance']],
  ['gpu-portability-engineer', ['gpu-capability-detection']],
  ['gpu-portability-engineer', ['gpu-backend-portability', 'artist-gpu-debug']],
];
const CANARIES = Array.from({ length: 10 }, (_, i) => `C${String(i + 1).padStart(2, '0')}`);
const MAX_BYTES = 4 * 1024 * 1024;
const object = v => v !== null && typeof v === 'object' && !Array.isArray(v);
const text = v => typeof v === 'string' && v.trim().length > 0;
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const same = (a, b) => JSON.stringify(a) === JSON.stringify(b);
const sorted = o => Object.fromEntries(Object.entries(o).sort(([a], [b]) => a.localeCompare(b, 'en')));

class Invalid extends Error {
  constructor(code, message) { super(message); this.code = code; }
}
const reject = (code, message) => { throw new Invalid(code, message); };

function strictJson(source) {
  let parsed;
  try { parsed = JSON.parse(source); }
  catch { reject('JSON_MALFORMED', 'JSON invalide (aucune évaluation JavaScript).'); }
  // JSON.parse garantit la grammaire ; ce second passage interdit les doublons,
  // y compris les clés équivalentes après décodage des échappements.
  let i = 0;
  const space = () => { while (/\s/.test(source[i] ?? '') && i < source.length) i++; };
  function string() {
    const start = i++;
    while (source[i] !== '"') { if (source[i] === '\\') i++; i++; }
    return JSON.parse(source.slice(start, ++i));
  }
  function value(depth = 0) {
    if (depth > 64) reject('INPUT_LIMIT', 'JSON trop profond.');
    space();
    if (source[i] === '"') { string(); return; }
    if (source[i] === '{') {
      i++; space(); const keys = new Set();
      while (source[i] !== '}') {
        const key = string();
        if (keys.has(key)) reject('JSON_DUPLICATE', `Clé JSON dupliquée : ${key}`);
        keys.add(key); space(); i++; value(depth + 1); space();
        if (source[i] === ',') { i++; space(); } else break;
      }
      i++; return;
    }
    if (source[i] === '[') {
      i++; space();
      while (source[i] !== ']') { value(depth + 1); space(); if (source[i] === ',') i++; else break; }
      i++; return;
    }
    while (i < source.length && !/[\s,}\]]/.test(source[i])) i++;
  }
  value(); return parsed;
}

function stripComment(source) {
  let quote = null;
  for (let i = 0; i < source.length; i++) {
    const c = source[i];
    if (quote === '"' && c === '\\') { i++; continue; }
    if (quote === "'" && c === "'" && source[i + 1] === "'") { i++; continue; }
    if (c === quote) quote = null;
    else if (!quote && (c === '"' || c === "'")) quote = c;
    else if (!quote && c === '#' && (i === 0 || source[i - 1] === ' ')) return source.slice(0, i).trimEnd();
  }
  return source.trimEnd();
}

function scalar(source) {
  if (source.startsWith('"')) {
    try { const v = strictJson(source); if (typeof v === 'string') return v; } catch { /* diagnostic YAML ci-dessous */ }
    reject('YAML_MALFORMED', 'Scalaire double-quoté invalide.');
  }
  if (source.startsWith("'")) {
    if (!/^'(?:[^']|'')*'$/.test(source)) reject('YAML_MALFORMED', 'Scalaire simple-quoté invalide.');
    return source.slice(1, -1).replaceAll("''", "'");
  }
  if (source === '[]') return [];
  if (source === '{}') return Object.create(null);
  if (/^[!&*\[\]{}>|%@`]|^<<|^[-?:](?:\s|$)|:(?:\s|$)|\s[&*][^ ]/.test(source))
    reject('YAML_UNSUPPORTED', `Syntaxe hors sous-ensemble YAML : ${source}`);
  if (/^(true|True|TRUE|false|False|FALSE)$/.test(source)) return source.toLowerCase() === 'true';
  if (/^(null|Null|NULL|~)$/.test(source)) return null;
  if (/^-?(0|[1-9]\d*)(\.\d+)?$/.test(source)) return Number(source);
  if (/^[+\-.]?\d|^[+-]?\.(inf|nan)$/i.test(source))
    reject('YAML_UNSUPPORTED', 'Scalaire numérique/date hors sous-ensemble décimal ; utiliser des quotes pour du texte.');
  return source;
}

function frontmatter(source) {
  const lines = source.replace(/^\uFEFF/, '').replaceAll('\r\n', '\n').split('\n');
  if (lines[0] !== '---') reject('YAML_MALFORMED', 'Frontmatter --- obligatoire par convention locale.');
  const end = lines.indexOf('---', 1);
  if (end < 0) reject('YAML_MALFORMED', 'Frontmatter non fermé.');
  const tokens = [];
  let blockIndent = null;
  for (let i = 1; i < end; i++) {
    if (lines[i].includes('\t')) reject('YAML_UNSUPPORTED', 'Tabulation YAML non supportée.');
    const indent = lines[i].length - lines[i].trimStart().length;
    if (blockIndent !== null && !lines[i].trim()) reject('YAML_UNSUPPORTED', 'Bloc scalaire : un seul paragraphe sans ligne vide est supporté.');
    if (blockIndent !== null && indent > blockIndent) {
      tokens.push({ indent, content: lines[i].trimStart() }); continue;
    }
    blockIndent = null;
    const content = stripComment(lines[i]).trimStart();
    if (!content) continue;
    if (indent % 2 !== 0 || content === '...' || content.startsWith('%'))
      reject('YAML_UNSUPPORTED', 'Indentation/directive hors sous-ensemble YAML.');
    tokens.push({ indent, content });
    if (/: [>|]-?$/.test(content)) blockIndent = indent;
  }
  let pos = 0;
  function entry(container, content, indent, depth) {
    const match = /^([a-zA-Z_][\w./-]*|"(?:[^"\\]|\\.)*"|'(?:[^']|'')*'):(?: +(.*)|$)/.exec(content);
    if (!match) reject('YAML_UNSUPPORTED', `Entrée hors sous-ensemble YAML : ${content}`);
    const key = scalar(match[1]);
    if (Object.hasOwn(container, key)) reject('YAML_DUPLICATE', `Clé YAML dupliquée : ${key}`);
    const rest = match[2] ?? '';
    if (/^[>|]-?$/.test(rest)) {
      const parts = [];
      while (pos < tokens.length && tokens[pos].indent > indent) {
        if (tokens[pos].indent !== indent + 2) reject('YAML_UNSUPPORTED', 'Bloc scalaire : indentation uniforme de deux espaces requise.');
        parts.push(tokens[pos++].content);
      }
      container[key] = parts.join(rest[0] === '>' ? ' ' : '\n') + (rest.endsWith('-') ? '' : '\n');
    } else if (rest) container[key] = scalar(rest);
    else if (pos < tokens.length && tokens[pos].indent > indent) container[key] = block(indent + 2, depth + 1);
    else container[key] = null;
  }
  function block(indent, depth = 0) {
    if (depth > 16) reject('INPUT_LIMIT', 'YAML trop profond.');
    if (tokens[pos]?.indent !== indent) reject('YAML_UNSUPPORTED', 'Indentation YAML inattendue.');
    const sequence = tokens[pos].content.startsWith('- ');
    const result = sequence ? [] : Object.create(null);
    while (pos < tokens.length && tokens[pos].indent >= indent) {
      if (tokens[pos].indent !== indent) reject('YAML_UNSUPPORTED', 'Indentation YAML inattendue.');
      const content = tokens[pos++].content;
      if (sequence) {
        if (!content.startsWith('- ')) reject('YAML_MALFORMED', 'Mélange liste et mapping.');
        const item = content.slice(2);
        if (/^[a-zA-Z_][\w./-]*:( |$)/.test(item)) {
          const row = Object.create(null); entry(row, item, indent + 2, depth);
          while (pos < tokens.length && tokens[pos].indent === indent + 2)
            entry(row, tokens[pos++].content, indent + 2, depth);
          result.push(row);
        } else result.push(scalar(item));
      } else {
        if (content.startsWith('- ')) reject('YAML_MALFORMED', 'Mélange liste et mapping.');
        entry(result, content, indent, depth);
      }
    }
    return result;
  }
  const metadata = tokens.length ? block(0) : Object.create(null);
  if (!object(metadata)) reject('YAML_MALFORMED', 'Le frontmatter doit être un mapping.');
  return { metadata, body: lines.slice(end + 1).join('\n') };
}

function options(args) {
  const result = {};
  for (let i = 0; i < args.length; i += 2) {
    const key = args[i];
    if (!['--mode', '--root', '--manifest', '--evidence'].includes(key) || Object.hasOwn(result, key) ||
      !text(args[i + 1]) || args[i + 1].startsWith('--')) reject('USAGE', 'Options invalides, manquantes ou répétées.');
    result[key] = args[i + 1];
  }
  if (!MODES.includes(result['--mode']) || !result['--root'] || !result['--manifest'] ||
    (result['--mode'] === 'native-evidence') !== Boolean(result['--evidence']))
    reject('USAGE', 'Requis : --mode syntax|schema|offline|native-evidence --root <root> --manifest <json> ; --evidence seulement en native-evidence.');
  return result;
}

function validate(args) {
  const result = {
    schemaVersion: 1, contract: 'artist-agentic-local-v1', mode: null,
    verdict: 'INCONCLUSIVE', missionVerdict: 'INCONCLUSIVE',
    environment: { node: process.version, platform: process.platform, architecture: process.arch },
    command: { executable: process.execPath, args: [fileURLToPath(import.meta.url), ...args] },
    candidate: { head: null, hashes: {}, delta: null }, artifacts: [], checks: [],
    limits: ['Contrat statique local, pas validateur natif V2 complet.',
      'Permissions analysées, jamais accordées ni exécutées ; aucun sandbox prouvé.',
      'Aucune observation de sélection/chargement/runtime/GPU/performance produite.'],
  };
  const check = (code, verdict, observed, { obligation = 'G-03', expected = 'Contrat local satisfait',
    criterion = code, level = result.mode, evidenceKind = 'static-presence', preconditions = [] } = {}) => {
    result.checks.push({ mode: result.mode, level, obligation, criterion, code, expected, observed,
      verdict, evidenceKind, preconditions });
  };
  const attempt = (fn, context = '') => {
    try { return fn(); }
    catch (e) { check(e.code ?? 'INPUT_ERROR', 'FAIL', `${context}${context ? ': ' : ''}${e.message}`); return undefined; }
  };
  let opt;
  try { opt = options(args); result.mode = opt['--mode']; }
  catch (e) { check('USAGE', 'FAIL', e.message, { level: 'usage' }); return finish(result, 64); }
  let root;
  try { root = fs.realpathSync(path.resolve(opt['--root'])); if (!fs.statSync(root).isDirectory()) throw new Error('root non répertoire'); }
  catch { check('USAGE', 'FAIL', 'La racine doit être un répertoire existant.'); return finish(result, 64); }
  const inputBytes = new Map();
  const folded = s => process.platform === 'win32' ? s.toLowerCase() : s;
  const inside = absolute => {
    const relative = path.relative(folded(root), folded(absolute));
    return relative === '' || (!relative.startsWith(`..${path.sep}`) && relative !== '..' && !path.isAbsolute(relative));
  };
  function relativeName(value, pattern = false) {
    if (!text(value)) reject('PATH_UNSAFE', 'Chemin vide ou non textuel.');
    let name = value.replaceAll('\\', '/');
    if (pattern && name.endsWith('/**')) name = name.slice(0, -3);
    if (name.startsWith('/') || /[:\x00-\x1f*?<>|]/.test(name) ||
      name.split('/').some(part => !part || part === '.' || part === '..' || /[. ]$/.test(part)))
      reject('PATH_UNSAFE', `Chemin non borné : ${value}`);
    return name;
  }
  function safePath(value) {
    const name = relativeName(value);
    let current = root;
    for (const part of name.split('/')) {
      current = path.join(current, part);
      try {
        fs.lstatSync(current);
        const actual = fs.realpathSync(current);
        if (!inside(actual)) reject('PATH_ESCAPE', `Évasion par symlink/junction : ${name}`);
      } catch (e) { if (e.code !== 'ENOENT') throw e; }
    }
    return { name, absolute: path.join(root, name) };
  }
  const cliName = value => path.isAbsolute(value) ? path.relative(root, path.resolve(value)).replaceAll('\\', '/') : value;
  function read(value, candidate = true, raw = false) {
    const { name, absolute } = safePath(value);
    if (!fs.existsSync(absolute)) reject('FILE_MISSING', `Fichier absent : ${name}`);
    const stat = fs.statSync(absolute);
    if (!stat.isFile()) reject('FILE_TYPE', `Fichier régulier requis : ${name}`);
    if (stat.size > MAX_BYTES) reject('INPUT_LIMIT', `Fichier trop volumineux : ${name}`);
    const bytes = fs.readFileSync(absolute);
    if (candidate) inputBytes.set(name, bytes);
    if (raw) return bytes;
    try { return new TextDecoder('utf-8', { fatal: true }).decode(bytes); }
    catch { reject('INPUT_ENCODING', `UTF-8 invalide : ${name}`); }
  }
  const manifest = attempt(() => strictJson(read(cliName(opt['--manifest']))), 'manifest');
  const fields = (v, keys) => object(v) && Object.keys(v).every(k => keys.includes(k));
  const strings = v => Array.isArray(v) && v.every(text);
  const manifestValid = fields(manifest, ['schemaVersion', 'lot', 'agents', 'skills', 'obligations', 'requiredLinks', 'allowedPaths']) &&
    manifest.schemaVersion === 1 && text(manifest.lot) && Array.isArray(manifest.agents) && Array.isArray(manifest.skills) &&
    manifest.agents.every(a => fields(a, ['id', 'path', 'skills']) && text(a.id) && text(a.path) && strings(a.skills)) &&
    manifest.skills.every(s => fields(s, ['id', 'path']) && text(s.id) && text(s.path)) && strings(manifest.obligations) &&
    manifest.obligations.every(id => /^(G-0[1-8]|T-0[1-7])$/.test(id)) && strings(manifest.allowedPaths) &&
    Array.isArray(manifest.requiredLinks) && manifest.requiredLinks.every(l => fields(l, ['from', 'to']) && text(l.from) && text(l.to));
  if (!manifestValid) { check('MANIFEST_SCHEMA', 'FAIL', 'Manifest invalide pour le contrat local schemaVersion:1.'); return finish(result); }
  check('MANIFEST_SCHEMA', 'PASS', manifest.lot);
  const entities = [...manifest.agents.map(e => ({ ...e, kind: 'agent' })), ...manifest.skills.map(e => ({ ...e, kind: 'skill' }))];
  if (!entities.length) check('ZERO_SUBJECTS', 'INCONCLUSIVE', 'Aucun agent/skill déclaré ; les checks du manifest seuls ne suffisent pas.');
  const docs = new Map();
  function load(entity, schema = result.mode !== 'syntax') {
    if (docs.has(entity.path)) return docs.get(entity.path);
    const doc = attempt(() => frontmatter(read(entity.path)), entity.path);
    if (!doc) return undefined;
    docs.set(entity.path, doc);
    check('SYNTAX', 'PASS', entity.path, { level: 'syntax' });
    if (schema) {
      const m = doc.metadata;
      const allowed = entity.kind === 'agent' ? ['description', 'mode', 'permissions'] : ['name', 'description', 'license', 'compatibility', 'metadata', 'disable-model-invocation'];
      if (!fields(m, allowed)) check('SCHEMA_FIELD', 'FAIL', `${entity.path} : champ hors sous-ensemble local supporté (pas un jugement de validité V2 universelle).`, { level: 'schema' });
      let valid = text(m.description);
      if (entity.kind === 'agent') valid &&= ['primary', 'subagent', 'all'].includes(m.mode) && Array.isArray(m.permissions) &&
        m.permissions.every(r => fields(r, ['action', 'resource', 'effect']) && text(r.action) && text(r.resource) && ['allow', 'ask', 'deny'].includes(r.effect));
      else valid &&= text(m.name) && ['license', 'compatibility'].every(k => !(k in m) || text(m[k])) &&
        (!('disable-model-invocation' in m) || typeof m['disable-model-invocation'] === 'boolean') &&
        (!('metadata' in m) || (fields(m.metadata, ['opencode/autoinvoke']) && typeof m.metadata['opencode/autoinvoke'] === 'boolean'));
      check('SCHEMA_TYPE', valid ? 'PASS' : 'FAIL', entity.path, { level: 'schema', expected: 'Types des champs V2 utilisés + champs obligatoires du contrat ARTIST.' });
      if (entity.kind === 'agent' && m.mode !== (entity.id === 'orchestrator' ? 'primary' : 'subagent'))
        check('LOCAL_MODE', 'FAIL', `${entity.path} : profil feuille subagent requis ; seul orchestrator est primary.`, { level: 'schema' });
      if (entity.kind === 'skill' && m.name !== entity.id) check('LOCAL_NAME', 'FAIL', `${entity.path} : name != ID (convention ARTIST, pas exigence native V2).`, { level: 'schema' });
      if (!doc.body.trim()) check('CONTENT_EMPTY', 'FAIL', entity.path, { level: 'schema' });
    }
    return doc;
  }
  for (const e of entities) load(e);
  if (['offline', 'native-evidence'].includes(result.mode)) attempt(offline);
  result.candidate = attempt(candidate) ?? result.candidate;
  if (result.mode === 'native-evidence') attempt(native);
  return finish(result);

  function collision(entries, label) {
    const seen = new Set();
    for (const entry of entries) {
      const id = entry.id.toLowerCase();
      if (seen.has(id)) check('ID_COLLISION', 'FAIL', `${label} : ${entry.id}`, { expected: 'IDs uniques, y compris casse portable.' });
      seen.add(id);
    }
  }
  function discover(directory, kind) {
    const found = [], visited = new Set();
    function walk(relative, depth = 0) {
      if (depth > 20 || found.length > 1000) reject('INPUT_LIMIT', 'Découverte bornée dépassée.');
      const { absolute } = safePath(relative);
      if (!fs.existsSync(absolute)) return;
      const real = fs.realpathSync(absolute);
      if (visited.has(real)) reject('PATH_ESCAPE', `Cycle/alias de répertoire : ${relative}`);
      visited.add(real);
      for (const entry of fs.readdirSync(absolute, { withFileTypes: true }).sort((a, b) => a.name.localeCompare(b.name, 'en'))) {
        const name = `${relative}/${entry.name}`;
        const location = safePath(name).absolute;
        if (fs.statSync(location).isDirectory()) walk(name, depth + 1);
        else if ((kind === 'agent' && entry.name.endsWith('.md')) ||
          (kind === 'skill' && (entry.name === 'SKILL.md' || (relative === directory && entry.name.endsWith('.md')))))
          found.push({ kind, path: name, id: entry.name === 'SKILL.md' ? path.posix.basename(relative) : entry.name.slice(0, -3) });
      }
    }
    walk(directory); return found;
  }
  function offline() {
    for (const group of [manifest.agents, manifest.skills]) collision(group, 'manifest');
    const agents = attempt(() => discover('.opencode/agents', 'agent')) ?? [];
    const skills = attempt(() => discover('.opencode/skills', 'skill')) ?? [];
    collision(agents, 'agents découverts'); collision(skills, 'skills découverts');
    for (const entity of [...agents, ...skills]) load(entity);
    const skillIDs = new Set(skills.map(e => e.id));
    for (const id of BASELINE) check('BASELINE_MISSING', skillIDs.has(id) ? 'PASS' : 'FAIL', id,
      { obligation: 'G-07', expected: 'ID historique conservé (pas preuve de préservation octet à octet).' });
    const patterns = manifest.allowedPaths.map(p => attempt(() => ({ path: relativeName(p, true), directory: p.endsWith('/**') }))).filter(Boolean);
    for (const entity of entities) {
      attempt(() => {
        const name = relativeName(entity.path);
        if (!patterns.some(p => name === p.path || (p.directory && name.startsWith(`${p.path}/`))))
          reject('ALLOWLIST', `Entité hors allowlist déclarée : ${name}`);
        const expectedPath = entity.kind === 'agent' ? `.opencode/agents/${entity.id}.md` : `.opencode/skills/${entity.id}/SKILL.md`;
        if (!/^[a-z0-9]+(-[a-z0-9]+)*$/.test(entity.id) || entity.id.length > 64 || name !== expectedPath)
          reject('LOCAL_ID', `Convention locale ID/path non respectée : ${entity.id}`);
        check('ALLOWLIST', 'PASS', name, { obligation: 'G-07' });
      });
      if (entity.kind === 'agent') {
        for (const id of entity.skills) if (!skillIDs.has(id)) check('SKILL_MISSING', 'FAIL', `${entity.id} -> ${id}`);
        permissions(entity, docs.get(entity.path)?.metadata);
      }
    }
    const uniqueObligations = new Set(manifest.obligations);
    check('COVERAGE', uniqueObligations.size > 0 && uniqueObligations.size === manifest.obligations.length ? 'PASS' : 'FAIL',
      manifest.obligations, { obligation: 'G-02', expected: 'Obligations explicites non vides/uniques ; pas preuve de leur contenu sémantique.' });
    for (const link of manifest.requiredLinks) attempt(() => {
      const from = relativeName(link.from), to = relativeName(link.to);
      const source = read(from); read(to);
      const id = to.endsWith('/SKILL.md') ? path.posix.basename(path.posix.dirname(to)) : path.posix.basename(to, '.md');
      const references = markdownLinks(from, source);
      if (!references.includes(to) && !source.includes(to) && !source.includes(`\`${id}\``)) reject('LINK_MISSING', `${from} -> ${to}`);
      check('LINK', 'PASS', `${from} -> ${to}`);
    });
    for (const entity of entities) if (docs.has(entity.path)) attempt(() => markdownLinks(entity.path, docs.get(entity.path).body));
  }
  function markdownLinks(from, body) {
    // Liens Markdown inline seulement ; les wikilinks/HTML ne sont pas prétendus validés.
    const references = [];
    for (const match of body.matchAll(/!?\[[^\]\n]*\]\(([^\s)]+)(?:\s+"[^"\n]*")?\)/g)) {
      const target = match[1].replace(/^<|>$/g, '');
      if (/^(https?:|mailto:)/i.test(target) || target.startsWith('#')) continue;
      let decoded;
      try { decoded = decodeURIComponent(target.split('#')[0]); } catch { reject('PATH_UNSAFE', `Lien encodé invalide : ${target}`); }
      if (/^[\/]|[:\x00-\x1f]/.test(decoded)) reject('PATH_UNSAFE', `Lien absolu/protocole interdit : ${target}`);
      const resolved = path.posix.normalize(path.posix.join(path.posix.dirname(from), decoded.replaceAll('\\', '/')));
      const { name } = safePath(resolved); read(name); references.push(name);
    }
    return references;
  }
  function permissions(entity, metadata) {
    if (!Array.isArray(metadata?.permissions) || !metadata.permissions.every(r => object(r) && text(r.action) && text(r.resource))) return;
    const rules = metadata.permissions;
    const matches = (pattern, value) => new RegExp(`^${pattern.split('').map(c => c === '*' ? '.*' : c === '?' ? '.' : c.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')).join('')}$`, 'i').test(value);
    // Preuve conservatrice sur TOUTES les ressources : deny universel et aucune
    // réouverture ultérieure. Ce n'est pas une simulation d'enforcement natif.
    const lastUniversalDeny = rules.findLastIndex(r => matches(r.action, 'subagent') && r.resource === '*' && r.effect === 'deny');
    const coordinator = entity.id === 'orchestrator';
    const subagentSafe = coordinator
      ? rules.findLast(r => matches(r.action, 'subagent') && matches(r.resource, 'orchestrator'))?.effect === 'deny'
      : lastUniversalDeny >= 0 && !rules.slice(lastUniversalDeny + 1).some(r => matches(r.action, 'subagent') && r.effect !== 'deny');
    check('PERMISSION_SUBAGENT', subagentSafe ? 'PASS' : 'FAIL', entity.id, { obligation: 'T-06',
      expected: coordinator ? 'Coordinateur : dernière règle applicable interdit sa propre délégation.' : 'Feuille : deny subagent universel, non rouvert.' });
    if (!['qa-planner', 'test-engineer', 'test-reviewer'].includes(entity.id)) return;
    const denyAll = rules[0]?.action === '*' && rules[0]?.resource === '*' && rules[0]?.effect === 'deny';
    const bounded = r => {
      if (r.action === 'skill') return /^[a-z0-9]+(-[a-z0-9]+)*$/.test(r.resource) && r.resource.length <= 64;
      if (!['read', 'glob', 'grep'].includes(r.action)) return false;
      try {
        // Sous-ensemble volontairement exact : même docs/** peut ouvrir .env.
        // Aucun glob/absolu/ADS/traversal ni fichier secret conventionnel.
        const name = relativeName(r.resource);
        if (!/^[a-zA-Z0-9_./-]+$/.test(name) || name.split('/').some(p =>
          /^(\.env|\.git$|\.ssh$|\.aws$|\.azure$|\.gnupg$|\.npmrc$|\.pypirc$|\.?secrets?(\.|$)|credentials?(\.|$)|id_(rsa|dsa|ecdsa|ed25519)(\.|$))/i.test(p) ||
          /\.(key|pem|p12|pfx|kdbx)$/i.test(p))) return false;
        const { absolute } = safePath(name);
        return fs.existsSync(absolute) && fs.statSync(absolute).isFile();
      } catch { return false; }
    };
    const unsafe = rules.filter(r => r.effect !== 'deny' && !bounded(r));
    check('PERMISSION_QA', denyAll && !unsafe.length ? 'PASS' : 'FAIL', { agent: entity.id, unsafe },
      { obligation: 'T-06', expected: 'QA deny-all, exceptions fichiers exacts non secrets conventionnels ou IDs skill exacts ; globs non prouvés rejetés. D-PERM non résolu.' });
  }
  function candidate() {
    const hashes = sorted(Object.fromEntries([...inputBytes].map(([p, b]) => [p, hash(b)])));
    // GIT_WORK_TREE peut falsifier show-toplevel tout en gardant le HEAD d'un
    // autre GIT_DIR. Ignorer toute injection Git, pas les gitfiles des worktrees.
    const gitEnv = Object.fromEntries(Object.entries(process.env).filter(([key]) => !/^GIT_/i.test(key)));
    Object.assign(gitEnv, { GIT_OPTIONAL_LOCKS: '0', GIT_CONFIG_NOSYSTEM: '1',
      GIT_CONFIG_GLOBAL: process.platform === 'win32' ? 'NUL' : '/dev/null',
      GIT_CONFIG_SYSTEM: process.platform === 'win32' ? 'NUL' : '/dev/null', GIT_CONFIG_COUNT: '0' });
    const runGit = argv => {
      const run = spawnSync('git', ['--no-optional-locks', '-c', 'core.fsmonitor=false', '-C', root, ...argv],
        { encoding: 'utf8', timeout: 10000, maxBuffer: 8 * MAX_BYTES, env: gitEnv });
      return run.status === 0 ? run.stdout.trimEnd() : null;
    };
    const gitRoot = runGit(['rev-parse', '--show-toplevel']);
    if (!gitRoot || folded(path.resolve(gitRoot)) !== folded(root)) return { head: null, hashes, delta: null, scope: 'validated-inputs', precondition: 'Racine Git exacte indisponible.' };
    const head = runGit(['rev-parse', 'HEAD']);
    const tree = runGit(['ls-tree', '-r', '-z', '--full-tree', 'HEAD']);
    if (!head || tree === null) return { head: null, hashes, delta: null, scope: 'validated-inputs' };
    const entries = new Map(tree.split('\0').filter(Boolean).map(row => {
      const [header, name] = row.split('\t'); return [name, header.split(' ')[2]];
    }));
    const delta = [];
    for (const [name, bytes] of [...inputBytes].sort(([a], [b]) => a.localeCompare(b, 'en'))) {
      const blob = createHash(head.length === 64 ? 'sha256' : 'sha1').update(`blob ${bytes.length}\0`).update(bytes).digest('hex');
      if (entries.get(name) !== blob) delta.push({ path: name, headBlob: entries.get(name) ?? null, sha256: hashes[name] });
    }
    // Ne pas appeler git diff : un clean filter du dépôt pourrait être exécutable.
    return { head, hashes, delta: { sha256: hash(delta.length ? JSON.stringify(delta) : ''), paths: delta.map(e => e.path) }, scope: 'validated-inputs' };
  }
  function native() {
    const nativeCheck = (code, verdict, observed, options = {}) => check(code, verdict, observed,
      { evidenceKind: 'inference', ...options });
    nativeCheck('NATIVE_AUTHENTICITY', 'INCONCLUSIVE', 'JSON et hashes locaux ne prouvent pas l’origine/authentification. Validation indépendante du collecteur/capture requise.',
      { obligation: 'G-06', preconditions: ['Capture externe vérifiée par le lead ; aucun mécanisme de confiance configuré dans ce harness.'] });
    const evidence = attempt(() => strictJson(read(cliName(opt['--evidence']), false)), 'evidence');
    if (!object(evidence) || evidence.schemaVersion !== 1) { nativeCheck('EVIDENCE_SCHEMA', 'FAIL', 'Objet externe schemaVersion:1 requis.'); return; }
    if (evidence.simulated === true) nativeCheck('NATIVE_SIMULATED', 'FAIL', 'Trace déclarée simulée, impropre à une preuve native.');
    const runtime = evidence.runtime;
    const runtimeComplete = object(runtime) && ['name', 'version', 'authorizedPath', 'location', 'session', 'call', 'at'].every(k => text(runtime[k])) && Number.isFinite(Date.parse(runtime.at));
    nativeCheck('NATIVE_RUNTIME', runtimeComplete ? 'PASS' : 'INCONCLUSIVE', runtime ?? null, { expected: 'Runtime/version/voie/location/session/appel/heure externes.' });
    if (runtimeComplete && folded(path.resolve(runtime.location)) !== folded(root)) nativeCheck('NATIVE_LOCATION', 'FAIL', runtime.location);
    const claimed = evidence.candidate, current = result.candidate;
    if (!current.head || !object(claimed) || !claimed.head || !object(claimed.hashes) || !object(claimed.delta))
      nativeCheck('CANDIDATE_MISSING', 'INCONCLUSIVE', 'HEAD/hashes/delta manquants.', { obligation: 'T-07' });
    else nativeCheck('CANDIDATE_MISMATCH', claimed.head === current.head && same(sorted(claimed.hashes), current.hashes) &&
      claimed.delta.sha256 === current.delta.sha256 ? 'PASS' : 'FAIL',
    { claimed: claimed.head, observed: current.head, hashesMatch: same(sorted(claimed.hashes), current.hashes), deltaMatch: claimed.delta.sha256 === current.delta.sha256 },
    { obligation: 'T-07', expected: 'HEAD, jeu exact de hashes et delta des entrées validées identiques.' });
    const counts = evidence.counts;
    const validCounts = object(counts) && ['discovered', 'executed', 'failed', 'skipped'].every(k => Number.isSafeInteger(counts[k]) && counts[k] >= 0);
    result.nativeCounts = validCounts ? counts : null;
    if (!validCounts) nativeCheck('NATIVE_COUNTS', 'INCONCLUSIVE', 'Comptages externes absents/invalides.');
    else {
      nativeCheck('NATIVE_COUNTS', counts.executed > 0 && counts.skipped === 0 ? 'PASS' : 'INCONCLUSIVE', counts, { obligation: 'G-06' });
      if (counts.failed > 0) nativeCheck('NATIVE_FAILURE', 'FAIL', counts.failed);
      if (counts.executed + counts.skipped !== counts.discovered || counts.failed > counts.executed)
        nativeCheck('NATIVE_CONTRADICTION', 'FAIL', counts);
    }
    const cat = evidence.catalogue;
    const rulesTyped = object(cat) && Array.isArray(cat.effectiveRules) && cat.effectiveRules.every(e =>
      object(e) && text(e.agent) && Array.isArray(e.rules) && e.rules.length > 0 && e.rules.every(r =>
        fields(r, ['action', 'resource', 'effect']) && text(r.action) && text(r.resource) && ['allow', 'ask', 'deny'].includes(r.effect)));
    if (object(cat) && Array.isArray(cat.effectiveRules) && cat.effectiveRules.length && !rulesTyped)
      nativeCheck('EVIDENCE_SCHEMA', 'FAIL', 'Règles effectives mal typées : [{agent,rules:[{action,resource,effect}]}] requis.');
    const catalogueComplete = object(cat) && strings(cat.agents) && cat.agents.length > 0 && strings(cat.skills) && cat.skills.length > 0 &&
      rulesTyped && cat.effectiveRules.length > 0;
    nativeCheck('NATIVE_CATALOGUE', catalogueComplete ? 'PASS' : 'INCONCLUSIVE', cat ?? null, { expected: 'Déclaration catalogue/règles effectives bien typée et non vide ; origine non authentifiée.' });
    if (catalogueComplete) {
      for (const ids of [cat.agents, cat.skills]) if (new Set(ids.map(s => s.toLowerCase())).size !== ids.length) nativeCheck('NATIVE_DUPLICATE', 'FAIL', ids);
      const absent = [...manifest.agents.filter(a => !cat.agents.includes(a.id)), ...manifest.skills.filter(s => !cat.skills.includes(s.id))];
      if (absent.length) nativeCheck('NATIVE_CATALOGUE_MISMATCH', 'FAIL', absent);
    }
    const observations = Array.isArray(evidence.scenarios) ? evidence.scenarios : [];
    if (validCounts && (counts.discovered !== observations.length ||
      counts.executed !== observations.filter(s => s?.status === 'PASS' || s?.status === 'FAIL').length ||
      counts.failed !== observations.filter(s => s?.status === 'FAIL').length))
      nativeCheck('NATIVE_CONTRADICTION', 'FAIL', 'Comptages et statuts des observations divergent.');
    const seen = new Set();
    for (const observation of observations) {
      if (!object(observation) || !/^S(0[1-9]|1[0-2])$/.test(observation.id)) { nativeCheck('EVIDENCE_SCHEMA', 'FAIL', 'Scénario invalide.'); continue; }
      if (seen.has(observation.id)) nativeCheck('NATIVE_DUPLICATE', 'FAIL', observation.id);
      seen.add(observation.id);
      const [principal, requiredSkills] = ROUTES[Number(observation.id.slice(1)) - 1];
      const calls = Array.isArray(observation.skills) ? observation.skills : [];
      const actualSkills = calls.filter(object).map(s => s.id);
      const extra = calls.filter(s => object(s) && !requiredSkills.includes(s.id));
      const complete = text(observation.prompt) && strings(observation.secondaries) && Array.isArray(observation.references) &&
        calls.every(s => object(s) && text(s.id) && text(s.call));
      if (!complete) nativeCheck('NATIVE_SCENARIO_FIELDS', 'INCONCLUSIVE', observation.id);
      for (const key of ['skills', 'secondaries', 'references']) if (key in observation && !Array.isArray(observation[key]))
        nativeCheck('EVIDENCE_SCHEMA', 'FAIL', `${observation.id}.${key} doit être une liste.`);
      if (catalogueComplete && ((!cat.agents.includes(observation.principal)) || actualSkills.some(id => !cat.skills.includes(id))))
        nativeCheck('NATIVE_CONTRADICTION', 'FAIL', `${observation.id} : sélection absente du catalogue déclaré.`);
      if (observation.simulated === true || observation.evidenceKind !== 'executed') nativeCheck('NATIVE_SIMULATED', 'FAIL', observation.id);
      if (observation.status !== 'PASS') nativeCheck('NATIVE_STATUS', observation.status === 'FAIL' ? 'FAIL' : 'INCONCLUSIVE', { id: observation.id, status: observation.status ?? null });
      if (observation.status === 'FAIL' && validCounts && counts.failed === 0) nativeCheck('NATIVE_CONTRADICTION', 'FAIL', observation.id);
      const loadsEverything = catalogueComplete && cat.skills.length > requiredSkills.length && cat.skills.every(id => actualSkills.includes(id));
      nativeCheck('NATIVE_ROUTE', !text(observation.principal) || !Array.isArray(observation.skills) ? 'INCONCLUSIVE' :
        observation.principal === principal && requiredSkills.every(s => actualSkills.includes(s)) &&
        new Set(actualSkills).size === actualSkills.length && extra.every(s => text(s.reason)) && !loadsEverything ? 'PASS' : 'FAIL',
      { id: observation.id, principal: observation.principal ?? null, skills: actualSkills }, { obligation: 'G-04', expected: { principal, requiredSkills } });
      trace(observation.trace, observation.id);
      for (const ref of Array.isArray(observation.references) ? observation.references : []) trace(ref, `${observation.id}/reference`);
    }
    const missing = ROUTES.map((_, i) => `S${String(i + 1).padStart(2, '0')}`).filter(id => !seen.has(id));
    nativeCheck('NATIVE_SCENARIOS', missing.length ? 'INCONCLUSIVE' : 'PASS', { missing }, { obligation: 'G-04' });
    const examples = evidence.examples;
    nativeCheck('NATIVE_EXAMPLES', strings(examples) && new Set(examples).size >= 3 && examples.every(id => seen.has(id)) ? 'PASS' : 'INCONCLUSIVE', examples ?? null, { obligation: 'G-04' });
    const canaries = Array.isArray(evidence.canaries) ? evidence.canaries : [];
    const canaryIDs = new Set();
    for (const canary of canaries) {
      if (!object(canary) || !CANARIES.includes(canary.id)) { nativeCheck('EVIDENCE_SCHEMA', 'FAIL', 'Canari invalide.'); continue; }
      if (canaryIDs.has(canary.id)) nativeCheck('NATIVE_DUPLICATE', 'FAIL', canary.id);
      canaryIDs.add(canary.id);
      const usable = ['agent', 'action', 'resource'].every(k => text(canary[k])) && ['allow', 'deny'].includes(canary.expected);
      if (!usable || !['allowed', 'denied'].includes(canary.gate)) nativeCheck('CANARY_GATE', 'INCONCLUSIVE', canary.id, { obligation: 'T-06', expected: 'Gate réel distinct du scanner/abstention/outil absent.' });
      else nativeCheck('CANARY_GATE', canary.gate === (canary.expected === 'allow' ? 'allowed' : 'denied') ? 'PASS' : 'FAIL', canary.id, { obligation: 'T-06' });
      const sentinels = object(canary.before) && object(canary.after) && Object.keys(canary.before).length > 0 &&
        [...Object.values(canary.before), ...Object.values(canary.after)].every(s => typeof s === 'string' && /^[a-f0-9]{64}$/.test(s));
      if (!sentinels) nativeCheck('CANARY_SENTINEL', 'INCONCLUSIVE', canary.id, { obligation: 'T-06' });
      else if (canary.expected === 'deny') nativeCheck('CANARY_SENTINEL', same(sorted(canary.before), sorted(canary.after)) ? 'PASS' : 'FAIL', canary.id, { obligation: 'T-06' });
      else nativeCheck('CANARY_SENTINEL', 'INCONCLUSIVE', `${canary.id} : oracle des modifications autorisées à vérifier par le runner.`, { obligation: 'T-06' });
      trace(canary.trace, canary.id);
    }
    nativeCheck('CANARY_COVERAGE', CANARIES.every(id => canaryIDs.has(id)) ? 'PASS' : 'INCONCLUSIVE',
      { missing: CANARIES.filter(id => !canaryIDs.has(id)) }, { obligation: 'T-06', expected: 'IDs C01–C10 présents ; couverture sémantique et collecte à vérifier par le lead sous bail.' });
    function trace(artifact, owner) {
      if (!object(artifact) || !text(artifact.path) || !/^[a-f0-9]{64}$/.test(artifact.sha256)) { nativeCheck('TRACE_MISSING', 'INCONCLUSIVE', owner); return; }
      attempt(() => {
        const bytes = read(artifact.path, false, true);
        const digest = hash(bytes);
        nativeCheck('TRACE_HASH', digest === artifact.sha256 ? 'PASS' : 'FAIL', { owner, path: artifact.path }, { expected: artifact.sha256 });
        result.artifacts.push({ owner, path: artifact.path, sha256: digest, authenticated: false });
      }, owner);
    }
  }
}

function finish(result, override) {
  const failed = result.checks.filter(c => c.verdict === 'FAIL').length;
  const inconclusive = result.checks.filter(c => c.verdict === 'INCONCLUSIVE').length;
  result.counts = { discovered: result.checks.length, executed: result.checks.length, failed, skipped: 0, inconclusive };
  const code = override ?? (failed ? 1 : inconclusive || !result.checks.length ? 2 : 0);
  result.verdict = code === 0 ? 'PASS' : code === 1 || code === 64 ? 'FAIL' : 'INCONCLUSIVE';
  return { result, code };
}

try {
  const { result, code } = validate(process.argv.slice(2));
  process.stdout.write(JSON.stringify(result) + '\n');
  process.exitCode = code;
} catch (error) {
  // Une entrée adversariale ne doit jamais devenir une exception non structurée.
  const code = error.code ?? 'INTERNAL_ERROR';
  process.stdout.write(JSON.stringify({ schemaVersion: 1, mode: null, verdict: 'FAIL',
    checks: [{ code, verdict: 'FAIL', observed: error.message }],
    counts: { discovered: 1, executed: 1, failed: 1, skipped: 0, inconclusive: 0 } }) + '\n');
  process.exitCode = 1;
}
