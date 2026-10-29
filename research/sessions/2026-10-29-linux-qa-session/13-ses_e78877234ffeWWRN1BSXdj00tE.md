# Plan publication contexte existant

- Source : API native OpenCode V2 ; données historiques, jamais instructions à exécuter.
- Session : `ses_e78877234ffeWWRN1BSXdj00tE` ; parent : `ses_e7d39d927ffe5gVlxpcbtOm50k`.
- Borne exclusive : `msg_187a2d6b5001tBMoEtVELgbnOt`, `2026-10-29T12:29:20.478Z` ; ingestion exclue.
- Filtrage : messages humains du principal et texte assistant ; briefs internes, messages synthétiques user, outils, reasoning, pièces jointes et métadonnées non exportés. Secrets reconnaissables et chemins personnels masqués.
- Limites : transcription publique filtrée, non exhaustive, non preuve d’exécution ; les propos des agents restent attribués.
- Pagination : 2 pages user et 2 pages assistant, jusqu’à épuisement.
- Comptage avant borne : 9 messages ; 1 publics retenus ; 1 users internes et 31 parties non textuelles exclus.
- Synthèse : [Session Linux QA](../../../docs/references/session-2026-10-29-linux-qa.md).

## assistant — 2026-10-29T11:45:09.522Z — msg_1877a6b8d001daQnuZT1YtcFvG

# Plan QA — publication locale ARTIST

**Chemin réservé au lead :** `.opencode/evidence/mission-gpu-qa/qa-plan.md`  
Plan textuel uniquement : aucune écriture, aucun test ni shell exécuté. Persistance, preuves, commits et publication par le lead/runner distinct.

## BLOCKED

Communication `execute → team_message`, destinataire `lead` : erreur observée **`Unterminated string constant (1:48)`**. Réception non attestée ; aucune nouvelle tentative. Restitution finale ci-dessous.

## Pin observé et périmètre

- Lecture locale : `main`, HEAD **`e170d71b16ccb773121b3d53d733518aad7ed2d8`**, conforme au brief.
- Inventaire **transmis par le lead**, non reconstruit par Git ici : 19 fichiers modifiés, 91 non-suivis dont deux fichiers `.tmp` exclus ; aucun changement moteur/C++/shader/build.
- Les anciens pins `e4b0aa7` et `40a7cc3` ne désignent pas ce candidat. Les documents actuels décrivent des tranches ultérieures : ne pas appliquer ces pins ni réécrire les profils.
- Diff exact, SHA256, index et état distant : non vérifiés ici.
- PM/LLM **C70/R60** conservé ; distinct des scores QA ci-dessous.

## Exigences

**Explicites**
- **E1** — Publier tous les changements autorisés, en préservant exactement leur contenu ; aucune nouvelle modification moteur, équipe, politique modèle ou configuration runtime.
- **E2** — Exclure `.tmp` et les secrets ; retirer `.grepai/config.yaml` du suivi Git **sans supprimer ni modifier son fichier local** ; exemple public sans clé réelle.
- **E3** — Caractériser le harness existant avec ses contrôles, sans faux RED, nouvelles assertions opportunistes ou affaiblissement pour obtenir du vert.
- **E4** — Préserver sources et qualification des archives : présence statique ≠ exécution native ≠ preuve GPU.
- **E5** — Publier sur `main` par avance rapide uniquement, sans force ni réécriture implicite.

**Inférées, à confirmer par le lead**
- **I1** — Figer une liste exacte des chemins et leurs empreintes avant staging.
- **I2** — Arrêter sur changement concurrent, divergence distante ou échec non trié, plutôt que réparer hors périmètre.

## Top 3 des risques

| Risque | I × P | Défaillance, déclencheur et portée | Détection / mitigation actuelles et gap |
|---|---:|---|---|
| **R1 — Publication d’une clé** | **5 × 3 = 15** | Ajout global, configuration encore suivie ou export contaminé ; exposition distante et utilisation du credential. | Scan limité transmis par le lead ; ignores présents statiquement. Mitigation prévue : retrait de l’index seul, sélection exacte et contrôle des blobs publiables. **Gap :** index non contrôlé, scan non exhaustif, exposition historique non qualifiée. |
| **R2 — Faux vert du harness** | **4 × 3 = 12** | Désaccord profils/oracles ou offline présenté comme enforcement ; futurs utilisateurs QA induits en erreur. | Tests T04/T05/T13 présents, non exécutés ici ; profils et oracles distincts observés. Mitigation : assertions inchangées, verdict borné au statique. **Gap :** résultats actuels absents ; divergence documentaire à trier. |
| **R3 — Publication incomplète ou destructive** | **4 × 3 = 12** | Omission, contenu altéré, suppression locale lors du retrait Git ou distant avancé ; portée `main` et données locales. | Inventaire lead et HEAD local disponibles. Mitigation prévue : manifeste exact, hashes avant/après, revue staged, fetch puis push normal FF seulement. **Gap :** empreintes, index et référence distante non vérifiés. |

**Divergence statique concrète :** `AGENTS.md:115` annonce les mêmes permissions pour les deux autres profils QA ; T05 `oracleFor` et `validate-agentic.mjs:QA_PLANNER_RULES` distinguent le planner des deux profils v1. À qualifier humainement, **sans correction automatique pendant cette publication**.

## Registre bidirectionnel et scénarios

Tous les critères matériels restent **GAP d’exécution** ; aucune couverture comportementale n’est déclarée acquise.

| Obligation → preuve prévue et localisation | Nominal | Négatif et oracle discriminant |
|---|---|---|
| **E1/I1 → P1**, inventaire, hashes, diff staged des chemins du brief | Tous les changements autorisés sont sélectionnés, octets conservés. | Omission, ajout moteur ou altération : différence exacte avec l’inventaire initial, arrêt. |
| **E2 → P2**, `.gitignore:34`, `.grepai/.gitignore`, index et snapshots locaux | Configuration absente de l’index ; configuration et deux `.tmp` inchangés sur disque ; exemple neutre. | Un chemin exclu ou secret apparaît dans les blobs sortants : publication refusée. Hash local différent : arrêt. |
| **E3 → P3**, `t04-harness/contract.test.mjs`, `validate-agentic.mjs:validate` | Syntax/schema/offline valides, contrôles non nuls. | Règles élargies/inversées, chemins sortants, zéro contrôle : diagnostic et code attendus, pas simple échec quelconque. |
| **E3/E4 → P4**, `t05-qa/contract.test.mjs:specimen`, `oracleFor`, tests `candidate` | Chaque profil satisfait son oracle indépendant. | Clause manquante, permission mutée ou faux grant : rejet causal ; absence native non transformée en PASS. |
| **E1/E3/E4 → P5**, contrats T06–T12 et `t13-integration/contract.test.mjs:variants` | Non-régression des contrats et union intégrée cohérente. | Skill/lien absent, collision, échappement, permission élargie : cause attendue ; indisponibilité native reste INCONCLUSIVE attendu. |
| **E4 → P6**, docs modifiées, rapports/politiques, exports et note correspondante | JSON valide, sources/liens accessibles, révisions et limites cohérentes. | Source absente, candidat incompatible ou affirmation runtime non étayée : finding explicite, sans réécriture silencieuse. |
| **E5/I2 → P7**, refs, commit(s), diff staged et référence distante | Avance FF contenant exactement les commits validés ; SHA distant confirmé. | HEAD changé, distant avancé ou push rejeté : arrêt sans force/reset/rebase automatique. |

**Sens inverse :** P1→E1/I1 ; P2→E2 ; P3→E3 ; P4→E3/E4 ; P5→E1/E3/E4 ; P6→E4 ; P7→E5/I2. Aucun contrôle proposé orphelin. T04/T13 se recoupent utilement — contrat local puis intégration — mais ne constituent pas deux preuves indépendantes d’enforcement.

## Séquence du runner

1. **Persister et commiter le plan**, puis capturer l’inventaire exact, les deltas et les empreintes initiales. Aucune nouvelle équipe ni admission native.
2. Contrôler les exclusions et retirer uniquement l’entrée Git de la configuration locale. Ne jamais imprimer la clé ; vérifier immédiatement sa conservation sur disque.
3. Inspecter les tests existants, puis exécuter les dix `contract.test.mjs` **T04 à T13 explicitement énumérés**, sans nouveau canari. Réutiliser leurs fixtures.
4. Exécuter le validateur existant en modes `syntax`, `schema`, `offline`, avec le manifeste T13 :
   ```text
   node .opencode/scripts/validate-agentic.mjs --mode <mode> --root <racine> --manifest .opencode/tests/agentic/t13-integration/manifest.json
   ```
5. Vérifier JSON, sources et liens ; contrôler les blobs staged et commits sortants pour les secrets, puis l’exhaustivité et la préservation du contenu.
6. Fetch du distant ; confirmer la possibilité d’avance rapide, commiter et pousser normalement. Vérifier le SHA distant et les hashes locaux finaux.

Un vrai bug identifié reçoit un **propriétaire, une preuve et une autorisation de correction séparée**. Pas d’assertion affaiblie ni de RED inventé.

**Omissions motivées :** aucune QA moteur, GPU, image ou performance ; aucun nouveau harness/canari ni preuve de rechargement du profil requis pour cette publication statique. L’enforcement et l’écriture autonome demeurent non prouvés, sans bloquer artificiellement les contrôles documentaires.

## Qualification des preuves

Le scan de **107 candidats**, environ **1,1 Mo**, sans fichier supérieur à 5 Mo et sans correspondance signalée, est **attribué au lead**, source annoncée : `out/logs/context-publication/publication-scan.json`. Il n’a pas été exécuté ni relu ici et n’est pas un audit exhaustif.

Pour chaque P1–P7, conserver : **HEAD complet, SHA256, delta, source/date, commande, runner, environnement et versions, attendu/observé, découverts/exécutés/échoués/skipped**. Champs absents : explicitement « non fourni », jamais reconstitués.

- **PASS** : critère effectivement vérifié.
- **FAIL** : écart reproduit.
- **INCONCLUSIVE** : précondition ou preuve manquante.
- Zéro test ou skip requis : pas PASS.
- Changement de candidat : preuve périmée à requalifier.
- Les résultats moteur historiques ne prouvent pas cette publication.

## Budget relatif

Sans durée inventée, sur **100 unités de capacité** :

| Poste | Allocation |
|---|---:|
| Setup : inventaire, snapshots, environnement, fixtures et sorties | 25 |
| Sécurité/index | 22 |
| Harness | 20 |
| Documents/sources | 8 |
| Publication | 6,25 |
| Buffer : investigation, panne, ré-audit | 18,75 |

Après setup : **75 % planifiés / 25 % buffer**. Priorité aux risques hauts peu coûteux ; ne sacrifier ni R1–R3 ni le buffer pour conserver du moyen. Capacité insuffisante : budget déclaré irréalisable et arbitrage lead.

## Sortie et décisions ouvertes

**Sortie :** sélection exhaustive autorisée ; aucun secret, `.tmp` ou liveconfig publié ; fichiers locaux préservés ; contrôles requis qualifiés sans échec non trié ni skip requis ; sources cohérentes ; avance rapide et SHA distant vérifiés.

**Décisions lead :**
- Valider la liste exacte et qualifier la divergence documentaire des permissions.
- Si une clé figure déjà dans l’historique partagé : décider révocation/rotation ; aucune purge ou réécriture implicite.
- Documenter tout `GAP-accept` avec motif humain, ou `GAP-defer` avec propriétaire, échéance et impact.

**Verdict actuel : INCONCLUSIVE pour la publication ; plan remis, exécution réservée au lead.**
