# Ré-audit indépendant T04-R — fd1e8cd

Date : 2026-10-16. correctness-reviewer / review-harness-fix, contexte fresh,
PM/LLM C80/R78, lecture seule. HEAD propre avant/après :
`fd1e8cdd6559ec05b26012bb59ed42f05dc73f18`.
Aucun test/build/canari/exécution GPU ni écriture, sous-agent ou mutation MCP.
Sept SHA256 source recoupés avec le rapport auteur ; log lead 103/103, zéro
skip/échec et hash `efa37438244115fb4fdc2faea49c140463c18adea4d82f7a4469cfa6329c26af`
recoupé. Ce n'est pas une nouvelle exécution indépendante des RED/GREEN.

## Fermetures bornées

- F3 : coordinateur distingué des feuilles, dernière règle subagent/orchestrator
  doit deny ; six cas discriminants et primaire invalide sur feuille conservés.
- F4 : injections Git/configs/hooks/templates reproduites puis neutralisées dans
  deux dépôts possédés ; racine et git-dir réels avant mutation, timeouts et
  nettoyage des seuls mkdtemp. Pas allégation d'incident utilisateur historique.
- F5 : candidate.head ne dépend plus des GIT_DIR/GIT_WORK_TREE hérités ; deux
  HEAD distincts et worktree Git légitime couverts, sans mutation des fixtures.
- Anciens inputs F1/F2 : corrigés et régressions causales, mais classes de défauts
  seulement partiellement fermées : deux résiduels ci-dessous.
- F6 : faux négatif T05 réel déjà exécuté par le lead, distinct des faux positifs
  suivants ; voir `qa-policy-boundary.md`. Le reviewer ne le ré-arbitre pas.

## F1-R — P2 — secret à suffixe .env accepté comme fichier exact

`validate-agentic.mjs:407-415`, permissions::bounded. Fixture inerte : fichier
`docs/production.ENV.local`, règles deny-all, read/*.ENV.* deny, puis
read/docs/production.ENV.local allow. Attendu : PERMISSION_QA FAIL/exit1 ; la
dernière règle rouvre un secret que le contrat interdit de blanchir.
Le regex actuel ne reconnaît `.env` qu'au début d'un composant ; le suffixe passe
relativeName/safePath/isFile, unsafe reste vide et le contrôle peut PASS/exit0.
Même classe production.env / production.Env.local. Démonstration statique,
pas fuite native exécutée. Conserver docs/guide.md nominal ; tester suffixes
et casses avec sentinelles inertes, jamais un vrai secret.

## F2-R — P2 — fallback plain accepte virgule et fractions signées

`validate-agentic.mjs:101-108`, scalar, puis load:285-291.
`description: ,item` commence par un indicateur YAML réservé ; `-.5` et `+.5`
sont numériques YAML core ou doivent être explicitement hors sous-ensemble.
La classe initiale omet la virgule et le garde numérique ne couvre pas signe+
point ; les trois valeurs tombent dans return source, string non vide acceptée.
Attendu : YAML_UNSUPPORTED/MALFORMED ou SCHEMA_TYPE approprié, jamais une
description string valide non quotée. Positifs quotés correspondants obligatoires.
Lecture statique seulement ; reproduction dynamique à l'auteur/runner autorisé.

## Sûreté et limites

Surfaces d'écriture relues : mkdtemp possédés uniquement, t.after/finally ;
argv Git fixes ; aucune fixture importée ou git diff/clean-filter ; junction et
suppression visent les fixtures. Aucun oracle existant affaibli dans le diff.
Hypothèses : Node/Git/PATH de confiance, fixtures fraîchement possédées sans
écrivain hostile, cfg locale créée par ces tests. Les canaux Git couverts ne
neutralisent pas tout NODE_OPTIONS/PATH ; nettoyage normal n'est pas une garantie
contre SIGKILL/crash machine ou descendants arbitraires encore actifs.

Verdict : PASS borné F3–F5 et anciens inputs F1/F2 ; **F1/F2 partiels**, F6 FAIL
consommateur. T13 et PASS global toujours bloqués. Aucune preuve API/image/perf.
Lead : transmettre ces scénarios déjà établis à l'auteur de la correction F6 ;
nouvelle expérience discriminante, pas simple répétition plus coûteuse de 103 tests.
