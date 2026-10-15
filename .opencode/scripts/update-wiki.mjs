import { existsSync, readFileSync, realpathSync } from "node:fs";
import { join } from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";

const root = realpathSync(fileURLToPath(new URL("../../", import.meta.url)));
const metadata = JSON.parse(readFileSync(join(root, ".wiki.json"), "utf8"));

function packageRoot() {
  if (process.env.QMD_PACKAGE_ROOT) return process.env.QMD_PACKAGE_ROOT;
  try {
    return fileURLToPath(new URL("../", import.meta.resolve("@tobilu/qmd")));
  } catch {
    if (process.env.APPDATA) {
      const global = join(process.env.APPDATA, "npm", "node_modules", "@tobilu", "qmd");
      if (existsSync(join(global, "dist", "index.js"))) return global;
    }
    throw new Error("QMD package not found. Set QMD_PACKAGE_ROOT to the installed @tobilu/qmd directory.");
  }
}

function canonical(path) {
  const value = realpathSync(path);
  return process.platform === "win32" ? value.toLowerCase() : value;
}

const installed = packageRoot();
const { createStore } = await import(pathToFileURL(join(installed, "dist", "index.js")).href);
const { enableProductionMode, getDefaultDbPath } = await import(pathToFileURL(join(installed, "dist", "store.js")).href);
enableProductionMode();
const dbPath = getDefaultDbPath(metadata.qmd_index);
if (!existsSync(dbPath)) throw new Error(`QMD index does not exist: ${dbPath}. Register the collection first.`);

// DB-only mode preserves the global YAML configuration and uses its existing
// collection registration. Embeddings are refreshed separately by the QMD CLI.
const store = await createStore({ dbPath });
try {
  const collection = (await store.listCollections()).find(item => item.name === metadata.qmd_collection);
  if (!collection) throw new Error(`Register QMD collection ${metadata.qmd_collection} first.`);
  if (canonical(collection.pwd) !== canonical(join(root, metadata.vault_path)) ||
      collection.glob_pattern !== metadata.qmd_pattern) {
    throw new Error("QMD collection path/pattern differs from .wiki.json; refusing to update a different checkout.");
  }
  const { needsEmbedding, ...result } = await store.update({ collections: [metadata.qmd_collection] });
  console.log(JSON.stringify({ collection: metadata.qmd_collection, ...result, needsEmbeddingGlobal: needsEmbedding }, null, 2));
} finally {
  await store.close();
}
