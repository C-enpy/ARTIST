# Revue indépendante du harness — candidat 8f0a2ca

Date : 2026-10-16. Auteur : correctness-reviewer / review-harness, contexte fresh.
PM/LLM C80/R78 ; modèle choisi par grille approuvée v1, sans override.
HEAD avant/après : `8f0a2ca6df6266ef8783dd8e9205a9e7f333cec6`, arbre propre.
Revue intégrale du validateur, huit tests/manifests, README, contrats et metadata.
Lecture seule : aucun test/build/GPU/probe/indexeur, aucune modification.

## Findings statiquement démontrés, pas encore reproduits par exécution

### F1 — P1 — Glob QA global accepté

`validate-agentic.mjs:395-400`, validate::permissions. Un profil test-reviewer
DENY-ALL puis read/resource `**` allow échappe au filtre qui ne rejette que le
littéral `*` : ni `..` ni `.env` dans la chaîne, unsafe vide, denyAll vrai.
PERMISSION_QA peut PASS offline alors que l'exception n'est pas bornée.
Attendu : rejet des glob-equivalents globaux/ambigus, jamais qualification
fail-closed. T03 §7, G-07/T-06. Pas de contournement natif exécuté revendiqué.
Régression : fixture QA existante, remplacer seul le masque read par `**` ;
status1 + PERMISSION_QA FAIL ; ajouter variantes globales et secrets/casse Windows.

### F2 — P2 — Scalaire YAML invalide accepté comme texte

`validate-agentic.mjs:101-108`, scalar, puis entry:147 et SCHEMA_TYPE:285.
`description: - item` reste string non vide et peut sortir exit0 en syntax/schema.
Même classe avec `description: texte:` ; variantes True/NULL conservées en string
au lieu de types YAML core reconnus ou d'un rejet explicite du sous-ensemble.
Attendu : YAML_MALFORMED/YAML_UNSUPPORTED ou SCHEMA_TYPE approprié, sans fallback.
Régressions nominal quoted string et négatifs ci-dessus, diagnostics causaux.

### F3 — P2 — Coordinateur primary traité comme une feuille

`validate-agentic.mjs:391-394`, permissions. Recherche inconditionnelle d'un
deny subagent universel, même pour orchestrator dont load:292 reconnaît pourtant
l'exception primary. Le vrai coordinateur interdit sa propre délégation
(`resource: orchestrator`), pas tous les spécialistes. Attendu : contrat séparé
du coordinateur, auto-délégation interdite, délégation légitime conservée.
Régression : fixture orchestrator/primary/self-deny valide → offline0 ; conserver
le négatif primary sur une vraie feuille. Les huit manifests initiaux ne couvrent
pas orchestrator : pas un échec d'union effectivement exécuté ici.

## F4 — P1 conditionnel — Isolation Git des fixtures

`t04-harness/contract.test.mjs:54-63`, fixture::gitRun. Hérite de tout env/config
Git et ne neutralise pas hooks/templates. GIT_DIR/WORK_TREE/INDEX_FILE peuvent
rediriger init/add/commit ; hooksPath/templates peuvent exécuter un code non relu.
Le reviewer n'a pas observé de GIT_* ni hooksPath/template configurés dans SON
processus ; cela ne garantit pas le runner lead. Aucune compromission locale
reproduite. **Pas de GO à l'ancienne suite T04 en environnement non maîtrisé.**

Avant run : environnement Git neutralisé, hooks/templates vides possédés,
vérification show-toplevel/git-dir sous la fixture AVANT add/commit, timeout.
Régression entre DEUX dépôts jetables possédés, jamais .git canonique : injection
GIT_DIR du second, sentinelles index/refs ; variante hook inerte et templates
possédés. Refus avant écriture ou neutralisation contrôlée, pas oracle affaibli.

## Périmètre et preuves

77 payloads dans allowlists disjointes, requiredLinks existants, fichiers réguliers
constatés statiquement. Les sept suites T06–T12 sont fs lecture/assertions et
mutations mémoire, sans subprocess/fetch/write/cleanup. T04 crée/retire seulement
ses mkdtemp .run-* et une junction vers une seconde fixture possédée, réserve F4.
NATIVE_AUTHENTICITY demeure INCONCLUSIVE ; FAIL prioritaire ; zero/skip non PASS.
validated-inputs ne certifie pas tous les fichiers exclus.

SHA256 consultés :
- validateur : 46138cab414d6c48662f11d685680cd5e3abbcb5bd576f808cf57cd99790e61e
- tests T04 : a85de38fc9cf56093918ca275a1587e810c450ce67429883e6b8e8a3cd9eb84f
- tests T10 : cf43485135c8964a9bb9fede4695ce29c8f036d6d97d0fe490fe91019eb84aaf
- tests T11 : 33bb70ae89bf66698e2d762208466453755545f1cc2b0696ea74a675e16d975e

Décision lead : corriger F1–F3 avec RED causal ; traiter F4 par expérimentation
isolée et durcissement local, aucun changement Git global. T13 et PASS dépendants
bloqués. La revue n'a exécuté aucune commande de test ni validé GPU/performance.
