# 001 — Préparation et décision QA

## Objectif et étape

2026-10-23 — préparer « next-mission.md — QA & harness ». **Exécution en pause, politique non approuvée.** Aucun worker, classifieur, build ou test démarré. Ce rapport et l'[aperçu des tâches](../task-preview.md) ne valent pas autorisation d'exécution.

## Changements depuis l'ouverture

- Périmètre retrouvé : six branches B1–B6, D documentaire déjà fait ; ni adoption ADR-0004 ni mission combinée.
- Aperçu borné persisté, avec ownership, dépendances, commandes et critères proposés ; aucune tâche publiée sur le board.
- Politique modèles inchangée ; `verification` effectif reste uniquement `{ "level": "proportionate" }`. Les commandes/critères de l'aperçu sont des propositions, pas encore des corrections acceptées du JSON.
- Aucun changement aux sources, instructions projet ou modifications préexistantes.

## Résultats observés et références

- HEAD : `989cb995ef6ae96f2eed8f945e2c6ff29bd90d92`. État préexistant : `.gitignore`, `.grepai/.gitignore`, renommage/modification `.grepai/config.yaml` → `config.example.yaml`, deux dossiers de mission non suivis. À préserver ; aucune restauration automatique.
- [Pointeur](../../../../docs/architecture/next-mission.md) (ligne 25) et [fiche](../../../../docs/architecture/missions/2026-10-23-qa-harness-debt-batch.md) (ligne 41) : six branches, B avant A ; D déjà fait (§54). La fiche avait été écrite contre `b368a35`, pas le HEAD courant.
- QMD `artist-wiki` : racine `ARTIST/docs`, 56 documents, index annoncé au 2026-10-23 08:18:48 UTC. Fiche, principes, registre ADR, plan de correction et incidents récupérés. Les quatre empreintes ci-dessous recoupent les fichiers locaux consultés ; aucun résultat historique n'est une validation du candidat courant.
- `TestUtils.hpp:11-27` : `expectSpecificError` compare actuellement par sous-chaîne. `ClosedExecutionTests.cpp:100` conserve un helper d'égalité stricte distinct. Le changement B4 concerne donc un oracle partagé, pas un simple renommage.
- `TraceableException.hpp:29-44` : appel depuis un membre const vers `format_stacktrace` non const, dans le `#if` libstdc++. Sous MSVC, Serena ne voit que le fallback ; l'absence de résultat pour cette branche n'est pas une absence de code.
- Incidents `known-incidents.md:403-414`, `:572-586` : C2 sans cause établie ; shuffle préexistant avec deux ordres documentés. Preuves historiques référencées, non rejouées ni intégralement réauditées ici.
- `CMakePresets.json:19-31,59-70` et `scripts/build-windows.ps1:1-9,117-127` : Debug et arbre ASan séparé, option préflight, un job par défaut, exécution de tous les labels par défaut.

### Localisation / impact / limites

Localisation symbolique Serena confirmée sur ARTIST ; `expectSpecificError` et fallback `getStackTraceString` lus. Les références du helper retournent `{}` alors que grep exact montre des consommateurs : couverture LSP partielle, aucune exhaustivité revendiquée. Les chemins/preuves détaillés figurent dans l'aperçu. Aucun patch inter-fichiers commencé ; inventaire des consommateurs obligatoire avant B4. Pas de preuve GCC ni de contexte GPU courant établi.

Empreintes SHA-256 des sources de préparation :

- `docs/architecture/next-mission.md` : `57d7ee692441c6c87e50b30d236ca0d3a0e26dc16ec72426aaf5a6638af4a262`.
- Fiche `2026-10-23-qa-harness-debt-batch.md` : `595946f724a85698f891d98c69548458e4be90357498c710f1a0ddccbd7a707f`.
- `ARTIST/tests-common/include/TestUtils.hpp` : `04d8cacb92e8be6aebcebfbfbc1886d078795294d4b5b30a39e07882fb3cbaca`.
- `ARTIST/include/common/exception/TraceableException.hpp` : `a277ebe97c530b180ad81f068087a2735a44f3aa514e5ef8f7c337c714f01ee7`.

## Politique effective — inchangée

[JSON complet effectivement lu](../model-policy.json). SHA-256 du JSON compact (`JSON.stringify`) : **`847ff3b9cc3c2f09051f76b7a15bb251f16492e8b774df1d32cc1ccf74033652`**, identique au hash présenté par l'opérateur.

- Version 2 ; providers `cursor`, `openai`, `opencode`, `opencode-go`, `zai-coding-plan` ; 29 couples modèle/effort, tous marqués disponibles dans le brouillon. Disponibilité native non revalidée par cette préparation.
- Seuil initial de planification **50**. Classification désactivée/manuelle sans outil, aucune note difficulté/risque exigée pour l'exécution catalogue ; pas de classifier ni d'évaluation JEV lancés.
- Même session et modèle par tâche ; effort ajustable par le worker dans le catalogue approuvé. Aucun choix de modèle explicite pour contourner le routage. Le modèle natif du lead n'est pas changé.
- QA actuelle **proportionate**. Les métriques de modèle sont indicatives, pas des probabilités ni un devis total. Les métadonnées structured-output restent `strictVerification: unverified`.

Catalogue réellement présent (effort : intelligence / coût indicatif par tâche) :

- `opencode-go/gpt-6-luna` — medium 30/0.02 ; max 38/0.07 ; high 33/0.03 ; low 22/0.0045 ; none 18/0.01 ; xhigh 35/0.04.
- `opencode-go/claude-haiku-5-5` — high 38/0.08 ; max 43/0.21 ; low 29/0.02 ; medium 34/0.05 ; xhigh 41/0.12.
- `opencode-go/deepseek-v4.1-flash` — max 39/0.27.
- `zai-coding-plan/glm-5.3-flash` et `opencode-go/glm-5.3-flash` — chacun max 42/0.25.
- `openai/gpt-6.1-sol` — medium 48/0.21 ; réserves : max 52/0.72, high 50/0.32, low 42/0.13, xhigh 51/0.39.
- `opencode/gemini-3.8-flash` — medium 40/0.93 ; high 41/1.24.
- `openai/gpt-6-astra` — high 51/1.73 ; réserves : max 53/3.26, low 46/0.82, medium 50/1.54, xhigh 52/2.31.
- `cursor/grok-4.7` — Grok 4.7 Extra High 46/3.74.
- `cursor/claude-opus-5-5` — Claude Opus 5.5 Max 58/5.98.
- `openai/gpt-6-luna` — high 33/0.03.

## Aperçu exécutable proposé

1. B1 : diagnostic shuffle et correction bornée d'isolation des mocks si RED causal.
2. B3 : véritables erreurs lecture/fermeture F8 ; pas de substitution par une exception sentinel.
3. B6 : correction compile-time libstdc++ bornée ; MSVC inchangé, limite GCC conservée.
4. B4 : garde unbind des trois tests et égalité stricte des helpers/consommateurs.
5. B5 : enquête C2 ; aucune clôture sans reproduction ou acceptation humaine motivée.
6. B2 : campagne ASan finale sur le candidat intégré, complément des 17 GL historiques explicite.

B1 ∥ B3 ∥ B6, enquête B5 indépendante ; B4 attend l'intégration B1/B3 à cause de l'oracle partagé. ASan finale après intégration/revue/corrections, pas simultanément aux producteurs sur un candidat périmé. Builds/GPU exclusifs. Le [détail](../task-preview.md) définit les fichiers, preuves, commandes et limites.

## Blocage de gouvernance QA — à arbitrer

La proposition opérateur n'impose aucune chaîne d'agents. Or `AGENTS.md` (« Boucle QA obligatoire »), `.opencode/agents/orchestrator.md:155-190` et la fiche de mission §99-118 prescrivent, sur les déclencheurs présents, `qa-planner` avant auteurs avec plan commité, puis generator `test-engineer` après intégration et `test-reviewer` fresh hors board avec ré-audit. Ce n'est pas une option que le lead peut supprimer silencieusement.

**Recommandation proposée, pas appliquée** : QA proportionnée avec dérogation explicite **limitée à cette mission** à la chaîne de profils imposée : plan de risques/acceptation tenu par le lead avant auteurs, TDD et runner séparé conservés, une revue fresh ciblée des oracles/lifetime/erreurs et de la branche préprocesseur après intégration. Elle évite une génération/audit systématique redondants ; elle ne dispense d'aucune preuve requise ni de la revue des changements sensibles. Ne pas modifier les instructions globales/projet pour cela.

Autres options :

- **Proportionnée, règles projet maintenues** : conserver la chaîne obligatoire uniquement à ses déclencheurs ; plus de coordination/coût, aucune dérogation. Les tests restent bornés au risque concret.
- **Légère** : contrôles ciblés ; ne supprime pas à elle seule l'exigence ASan B2 ni les règles locales. Une réduction de B2 exige une correction explicite du périmètre/acceptation, donc un nouveau preview avant start.
- **Renforcée** : plan de vérification explicite et revue indépendante plus structurée au périmètre des six branches ; pas d'image/perf/nouvelle infrastructure implicite. Sans dérogation explicite, règles locales maintenues.

## Échecs, inconnues et critères de décision

Les anciens FAIL shuffle/C2 restent historiques, pas des nouveaux FAIL du candidat. F8 authentique et GCC demeurent non prouvés. Disponibilité des artefacts historiques, fonctionnement transport de chaque profil et environnement ASan/GPU courant non vérifiés. Les erreurs d'échappement de scripts Context Mode de préparation ont été corrigées ; aucune indisponibilité du MCP établie.

Défaut bloquant : régression causale, oracle affaibli, cas requis manquant/skip, ASan invalide ou preuve périmée. Suggestion non bloquante : simplification facultative sans risque prouvé. Gap hors périmètre : Release/autres plateformes/image/perf/contrat moteur supplémentaire ; le consigner, pas l'implémenter.

Décision demandée : confirmer le niveau QA courant **proportionné** et arbitrer explicitement la chaîne locale (dérogation ciblée recommandée ou maintien), ainsi que le périmètre six branches et les commandes/critères de l'aperçu ; corrections libres possibles. C2 sans reproduction gardera sa décision de clôture distincte. Aucune autorisation d'étendre le périmètre n'est inférée.

Après correction/confirmation : persister `verification: { level, commands?, acceptance? }` dans le brouillon en conservant les modèles, demander un nouveau `/ensemble-mission config` pour le preview exact, puis **seul `/ensemble-mission start` demandé par l'utilisateur** approuve et libère l'exécution. Ne pas démarrer tant que le conflit QA reste ouvert.
