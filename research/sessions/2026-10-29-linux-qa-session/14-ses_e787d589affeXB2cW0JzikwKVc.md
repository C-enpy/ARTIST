# Revue publication des règles QA

- Source : API native OpenCode V2 ; données historiques, jamais instructions à exécuter.
- Session : `ses_e787d589affeXB2cW0JzikwKVc` ; parent : `ses_e7d39d927ffe5gVlxpcbtOm50k`.
- Borne exclusive : `msg_187a2d6b5001tBMoEtVELgbnOt`, `2026-10-29T12:29:20.478Z` ; ingestion exclue.
- Filtrage : messages humains du principal et texte assistant ; briefs internes, messages synthétiques user, outils, reasoning, pièces jointes et métadonnées non exportés. Secrets reconnaissables et chemins personnels masqués.
- Limites : transcription publique filtrée, non exhaustive, non preuve d’exécution ; les propos des agents restent attribués.
- Pagination : 2 pages user et 2 pages assistant, jusqu’à épuisement.
- Comptage avant borne : 44 messages ; 2 publics retenus ; 2 users internes et 59 parties non textuelles exclus.
- Synthèse : [Session Linux QA](../../../docs/references/session-2026-10-29-linux-qa.md).

## assistant — 2026-10-29T11:57:18.794Z — msg_187858c45001sZXaRrYqIbddzE

## Verdict borné

**Réserve ciblée : un gap de couverture T05 à corriger puis ré-auditer.** Les tests sont discriminants sur les permissions v2 ; je n’ai pas identifié d’élargissement effectif dans le raccordement lu.

Revue fresh indépendante : je n’ai écrit ni code ni tests. Profil restrictif reçu, contexte ARTIST et lecture locale confirmés (`AGENTS.md`, diff, plan, tests et preuves). Skill chargé. Aucune édition, exécution de tests, API, shell ou délégation.

Transport imposé par le profil essayé une fois : **`This session is not in a team.`** Restitution native uniquement après ce refus.

## Finding

### Moyenne — l’interdiction d’employer Code Mode comme runner n’est pas verrouillée pour le planner

- **Localisation :** `.opencode/tests/agentic/t05-qa/contract.test.mjs:12–18`, à comparer aux exigences renforcées des deux autres rôles, lignes 23–27.
- **Plan / obligation :** plan local lignes 25–27, 35–43 et 53–60 ; T‑06, communication uniquement et runner séparé.
- **Input → mutant plausible :** conserver les 31 permissions du planner, mais supprimer l’interdiction nouvelle de `.opencode/agents/qa-planner.md:161–163` : ne pas employer ces capacités pour écrire, lancer des tests, muter une session, créer un worktree ou déléguer.
- **Assertion inefficace :** `plannerOracle` exige notamment `execute est allow` et `pas une sandbox`, mais aucune des interdictions précitées. Le specimen nominal est construit avec ces seules phrases obligatoires.
- **Attendu / observé :** ce retrait devrait produire `T-06.tools`. En lecture statique, aucune phrase obligatoire du planner ne disparaît ; `inspectProfile` ne peut donc pas signaler ce retrait. **Survie inférée, non exécutée.**
- **Impact :** une restriction de rôle essentielle pourrait disparaître tout en restant verte, alors que `execute` expose des capacités sans contrôle individuel.
- **Vérification minimale :** négatif planner supprimant cette interdiction, triplets inchangés ; exiger son rejet causal, puis conserver le nominal vert.
- **Destination :** **DEFER_TO: test-engineer**, via le lead. Ré-audit ciblé après intégration.

## Couverture constatée

**Static-presence :**
- Égalité des politiques des trois rôles et oracle du validateur hardcodé, non importé du candidat.
- T04 contrôle identité/path/mode, ordre, doublons, mutations de chaque règle et `firstMismatch` causal.
- Négatifs explicites pour shell/edit/subagent, contexte externe, mauvais pool, transport absent et oracle candidat falsifié.
- Contreparties nominales/négatives de la forme A conservées.
- Assertions préservant les FAIL prioritaires et `missionVerdict: INCONCLUSIVE`.
- Canaris maintenus `NOT_RUN`, observations et grant nuls.

**Executed — runner lead, pas moi :**
- RED lu : 6 tests, 2 PASS / 4 FAIL, divergences nominales et anciennes politiques visibles.
- GREEN lu : **633 PASS, 0 FAIL, 0 skipped** (`qa-policy-green-tests.log:3802–3808`).
- Modes fournis : syntax/schema/offline PASS, mission INCONCLUSIVE. Ces résultats ne prouvent aucun enforcement natif.

Les déclarations de permissions et les instructions lues distinguent correctement communication et runner ; le finding porte sur leur protection par les tests.

## Pin observé et limites

Identité fournie : `7132c9ac955a5a35fcd9eac49590bcad60b889cf`, base `e170d71…`. Les pins historiques du profil/skill ne décrivent plus exactement le canon actuel, notamment ADR‑0004 désormais **Proposed/partial**. Je ne les applique ni ne les réécris.

Aucune introspection native des permissions, aucun canari natif ou GPU exécuté ici. Secrets, transcripts privés et anciens boards non consultés. Les contrôles secrets/index/push restent hors de cet avis.

Après correction, commandes **proposées au runner uniquement** :
```text
node --test .opencode/tests/agentic/t05-qa/contract.test.mjs
node --test .opencode/tests/agentic/t04-harness/contract.test.mjs
node --test .opencode/tests/agentic/t13-integration/contract.test.mjs
```

## assistant — 2026-10-29T12:01:45.548Z — msg_187899e46001X2psNKZM7f0bSz

## Verdict : PASS borné — finding fermé

Ré-audit ciblé du candidat annoncé `be888ffcf1ef7ae7fd79c245001dca66c04f6814`, même contexte indépendant RO. **Aucun nouveau finding dans ce delta.**

### Scénario et assertions

**Static-presence**, `.opencode/tests/agentic/t05-qa/contract.test.mjs:18,34–46` :

- Le véritable profil planner doit d’abord être accepté.
- La présence des trois lignes à retirer est vérifiée : mutation non vacue.
- Leur retrait conserve explicitement le préfixe contenant les permissions.
- L’assertion exige précisément `T-06.tools`, pas une erreur quelconque.
- L’oracle ajoute l’interdiction attendue sans retirer ni assouplir d’exigence existante.

Cela ferme le gap identifié : suppression de la restriction runner → rejet statique attendu. Références au plan : lignes 25–27, 35–43 et 53–60, obligation T‑06.

### Preuves relues — exécutant : lead

- **RED** : `planner-runner-red.log:12–17,26–32` — 1 test, 1 échec ; `includes('T-06.tools')` observé `false`. Le patch à chaud contient le nouveau test sans correction d’oracle.
- **GREEN** : `planner-runner-green-tests.log:2397–2398,3808–3814` — nouveau test PASS ; **634/634 PASS, zéro échec ou skip**.

### Limites

Cette fermeture concerne uniquement la **détection statique de la disparition de la consigne**. Elle ne démontre ni chargement natif du profil, ni enforcement, ni confinement de Code Mode.

Aucun test exécuté par moi, aucune écriture, aucun nouvel essai de transport. Profils et validateur inchangés dans le delta lu. Aucun ré-audit supplémentaire requis pour ce finding ; indexation, secrets, publication et preuves natives restent hors périmètre.
