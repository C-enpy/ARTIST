# 41 — Context Mode connecté, outils non exposés au catalogue de ce tour

2026-10-25. Diagnostic demandé par l'opérateur après capture montrant le MCP activé et vert dans cette session. Aucun test moteur ni campagne relancé ; aucune configuration ou installation modifiée.

## Faits observés

- Recherche Code Mode sans filtre de namespace (`ctx_execute`, `context`) : aucun outil MCP correspondant. Recherche par namespaces context-mode et qmd : listes vides.
- Configuration globale effectivement lue : serveur context-mode local déclaré, sans disabled ; commande Node/preload/start.mjs existante. Configuration projet ARTIST ne désactive pas ce serveur.
- Permissions globales lues : execute et context-mode_*/context_mode_* allow ; profil orchestrator ne déclare qu'un deny de sous-délégation vers orchestrator, pas de deny MCP.
- Premier appel `opencode mcp list` : CLI absent du PATH, **pas une panne MCP**. Chemin du CLI réellement découvert par les processus OpenCode : `C:/Users/djden/AppData/Roaming/ai.opencode.desktop/cli/2.1.4-x64/opencode-cli.exe`.
- Ce CLI lancé avec `mcp list`, depuis ARTIST : **context-mode connected**, QMD et autres MCP attendus également connected. Plusieurs processus Node context-mode enfants du service sont présents ; aucun processus arrêté.
- Appel natif `list_mcp_resources({server:'context-mode'})` réussi, resources/templates vides. Cela n'établit pas l'absence d'outils : ressources et outils sont deux interfaces distinctes.
- Capture opérateur concordante avec cet état connecté/activé.

## Conclusion corrigée

L'indisponibilité est celle du **catalogue d'outils fourni à Code Mode dans le contexte actuel**, pas une désactivation ou une panne du serveur Context Mode. Les anciennes formulations « absent » doivent être lues comme « non exposé au catalogue », jamais comme une preuve que le MCP ne tourne pas. QMD présente la même divergence lors de cette vérification.

Cause précise non établie : catalogue conservé après redémarrage, filtrage ou défaut de synchronisation restent hypothèses. Aucun motif constaté pour installer/upgrader Context Mode, éditer ses permissions, basculer tous les MCP ou redémarrer le service.

## Expérience minimale suivante

Rechargement natif `/reload` dans **la même session**, puis nouvelle découverte ciblée et, si l'outil réapparaît, un appel minimal de contrôle. Le guide OpenCode V2 chargé documente `/reload` comme rechargement complet des configurations/outils. Aucun outil reload n'est exposé au lead : ne pas fabriquer un appel API ou une commande shell qui modifierait arbitrairement la session. Cette expérience n'a pas encore été exécutée et sa réussite n'est pas présumée.

Préserver mission, workers, worktrees et preuves. Pas de nouveau chat imposé, pas de nouvelle mission ni de rerun des555 tests. Campagne finale toujours suspendue jusqu'à restitution effective des outils et vérification minimale.
