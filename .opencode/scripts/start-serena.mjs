import { existsSync, realpathSync } from "node:fs";
import { homedir } from "node:os";
import { join } from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";

// Bind the pooled backend to this checkout, including when it is a worktree.
const root = realpathSync(fileURLToPath(new URL("../../", import.meta.url)));
const proxy = join(homedir(), ".config", "1mcp", "serena-proxy.mjs");

if (!existsSync(proxy)) {
  console.error(`ARTIST: install the existing 1MCP Serena proxy at ${proxy}`);
  process.exit(1);
}

process.chdir(root);
// Resolve the home path in Node rather than interpolating Windows backslashes
// into JSONC. The installed proxy owns startup and the unmodified stdio stream.
await import(pathToFileURL(proxy).href);
