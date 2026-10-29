# 01 — Préparation bornée QA & harnais

Date : 2026-10-24. État : proposition ; exécution suspendue, politique non approuvée.

## Objectif et état observé

Instruire les six findings de `docs/architecture/missions/2026-10-23-qa-harness-debt-batch.md` avant ADR-0004, sans refonte moteur ni extension de plateforme. Réussite : diagnostics causaux, correctifs minimaux prouvés, campagnes sur le candidat intégré et registre fidèle aux résultats.

- HEAD observé : `989cb995ef6ae96f2eed8f945e2c6ff29bd90d92`.
- Diff préalable inspecté : `.gitignore`, `.grepai/.gitignore`, renommage/configuration exemple GrepAI ; trois répertoires de missions non suivis. Ces changements sont hors ownership et seront préservés. Aucun diff moteur observé.
- Sources lues : `AGENTS.md`, principes, registre ADR, plan de correction, pointeur et fiche de mission. Recherche QMD dans `artist-wiki`, récupération de la fiche `#595946`, lignes 75–135, cohérente avec le fichier local.
- ADR pertinents : 0003 et 0005 Accepted/partial ; 0008 vérifié Windows Debug seulement ; 0002 Proposed/partial. 0001/0004/0006/0007 restent Proposed. Aucune adoption ni promotion prévue.
- Aucun worker, classifier, build ou test lancé. Aucun changement de source. Ce rapport est le seul livrable ajouté pendant la préparation.

## Politique réelle et proposition QA

Fichier réel : [model-policy.json](../model-policy.json), version 2, 29 entrées modèle/effort.

- Providers : `cursor`, `openai`, `opencode`, `opencode-go`, `zai-coding-plan`.
- Seuil initial : 50 ; `classificationBeta: 0.35` ; classification non lancée.
- QA persistée : `verification: { "level": "proportionate" }` ; aucune commande/acceptation personnalisée persistée.
- Même session et modèle par tâche ; ajustement du raisonnement seulement dans le catalogue approuvé. Aucun modèle forcé au spawn.
- Hash présenté par l'opérateur : `847ff3b9cc3c2f09051f76b7a15bb251f16492e8b774df1d32cc1ccf74033652` ; pas une approbation et pas recalculé ici.

**Conflits à résoudre avant start :** la politique générique « pas de pipeline QA obligatoire » et « aucun score requis » ne supprime pas les obligations ARTIST. Les instructions de coordination imposent PM/LLM, deux scores manuels et plancher développement 70 ; la fiche impose déjà une boucle QA pour B1–B3. Les instructions de coordination imposent aussi QA planner avant écriture, plan commité avant worktrees auteurs, generator après intégration et audit fresh hors board. Les trois QA restent en lecture seule, sans execute/shell ; les tests sont exécutés par le lead. Le texte local d'AGENTS autorisant execute QA diverge des instructions de coordination : ne pas élargir les permissions.

Recommandation : garder **proportionate**, avec ces obligations ciblées, sans infrastructure supplémentaire. **Light** limiterait les contrôles optionnels, pas les obligations. **Reinforced** ajouterait un plan/revue au-delà du minimum utile, à borner explicitement. Aucun changement silencieux de politique n'est proposé.

## Aperçu des tâches — non publiées

Plan préparatoire local : aucun PM worker autorisé avant approbation. Les estimations B1–B6 ci-dessous reprennent la fiche sélectionnée ; validation PM/lead avant publication. Ce ne sont ni des probabilités ni un appel classifier.

1. **Q — plan QA ciblé**, `qa-planner`, contexte fresh, lecture seule. Déclencheurs, nominal/négatif, oracles et gaps pour les six branches ; distinguer exigences explicites/inférées. Lead persiste et commit le plan avant les worktrees auteurs. Estimation PM/LLM proposée C45/R50, sans développement.
2. **B1 — shuffle**, `cpp-engineer`, fresh. C70→70/R55. Ownership fixtures/tests Loader et ShaderLoader, confirmé avant admission ; reproduction seeds `20261017` et `7`, hypothèse lifecycle/isolation. Pas de changement moteur sans RED causal. Deux essais sans information nouvelle imposent une nouvelle hypothèse/expérience.
3. **B3 — F8 réel**, `cpp-engineer`, fresh. C70→70/R50. Ownership loaders et tests associés : établir une vraie `ifstream::failure` et une fermeture échouée ; correction minimale seulement si défaut prouvé. Vérifier le chevauchement des tests avec B1 ; si même fichier, réunir les tranches sous un propriétaire ou les séquencer après intégration.
4. **B4 — unbind/helpers**, `gpu-engineer` pour le contrat des tests GL, fresh. C70→70/R45. `ARTIST/component-tests/src/graphic/shader/ProgramTests.cpp` : les trois tests identifiés ; helpers/ViewTests à localiser exactement avant publication. Réutiliser la garde existante, conserver l'égalité stricte des erreurs ; pas de nouveau framework ni RAII moteur exhaustive. Caractérisation avant refactoring ; RED causal si correction comportementale.
5. **B5 — C2**, `build-engineer`, fresh. C65→70/R45. Reproduction et preuves isolées de la baseline 20:35 ; aucun changement production implicite. Sans reproduction, proposer une fermeture motivée à l'opérateur ; conserver OPEN/DEFER tant qu'elle n'est pas acceptée.
6. **B6 — stacktrace**, `cpp-engineer`, fresh. C70→70/R50. `ARTIST/include/common/exception/TraceableException.hpp` et `ARTIST/tests/src/common/exception/TraceableExceptionTests.cpp`. Confirmer le défaut conditionnel libstdc++ ; correction compile-time bornée, MSVC inchangé. Aucun GCC installé ou nouvelle plateforme ; preuve GCC absente explicitement consignée.
7. **I — intégration**, lead. Arrêt/merge natifs, contrôle du diff et des preuves avant publication des consommateurs. Références Serena requises avant modification des symboles consommés ; couverture C++ et racine à confirmer à ce stade. Le présent plan n'est pas une analyse d'impact symbolique.
8. **B2 — campagne finale**, `build-engineer` propose sélection/protocole ; lead exécute. C65→70/R50. ASan sur le candidat intégré, sélection des 17 GL consignée avant run et découverte vérifiée. Préflight/outillage possible plus tôt, mais aucune campagne antérieure ne vaut preuve du candidat final.
9. **R/G puis V**, revue correctness fresh si correctifs lifetime/erreurs ; generator `test-engineer` après intégration pour gaps uniquement, puis `test-reviewer` fresh hors board et ré-audit si nécessaire. Propositions lecture seule, lead applique/exécute ; bug production reste rouge, maximum deux réparations healer. Estimations provisoires R C55/R55 ; G développement C60→70/R50 ; V C50/R50. Revue et analyse de gaps parallèles seulement sur candidat stable, sans exposer les justifications des auteurs au reviewer.
10. **Clôture**, lead : incidents, preuves ADR et pointeur documentaire après vérification seulement ; réindex du seul vault ARTIST. Aucun PASS global si échec ou précondition manquante.

## Graphe et ressources

`Décisions → PM/Q → commit QA → branches prêtes → intégration → B2 final + R/G → intégration tests éventuels → V → documentation`.

B1/B3/B4/B5/B6 peuvent enquêter en parallèle. Les écritures B1/B3 attendent une preuve de fichiers disjoints ; sinon séquence réelle ou propriétaire commun. B5/B2 partagent les ressources build : bail exclusif par arbre de sortie. Un seul build/indexeur par arbre, un job par défaut. Tests GL sérialisés (`artist_opengl`), aucune concurrence GPU entre campagnes. B2 final attend toutes les mutations de son candidat ; toute mutation ultérieure pertinente invalide les preuves concernées. Les consommateurs ne sont publiés qu'après intégration, avec vrais IDs et HEAD/diff disponibles.

## Commandes et acceptation proposées — non exécutées

Entrées vérifiées dans `scripts/build-windows.ps1` (paramètres lignes 1–9) :

- `powershell -NoProfile -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -TestLabel all`
- `powershell -NoProfile -File scripts/build-windows.ps1 -Configuration Debug -Jobs 1 -AddressSanitizer -AsanPreflightOnly`
- Campagne ASan ciblée à figer après découverte : arborescence séparée, sélection explicite avant exécution, commandes exactes du protocole existant ; ne pas remplacer les 17 GL par un run aveugle.
- Shuffle : binaire et filtre exacts déterminés après découverte ; seeds 20261017 et 7, contrôles de contamination et résultat par répétition.
- Après clôture documentaire : `node .opencode/scripts/update-wiki.mjs`, puis `qmd embed -c artist-wiki`.

Acceptation proposée : nombres découverts/exécutés concordants, zéro échec et zéro skip requis sur Windows Debug unit+GL du candidat ; ASan délimité avec options et sélection explicites ; RED causal avant correctif, GREEN/non-régression après intégration ; chaque finding traité, fermeture motivée acceptée ou DEFER avec propriétaire. Une panne d'environnement n'est pas RED. GCC natif, Release, images, performance, nouveaux backends et ADR-0004 sont exclus.

## Inconnues et décisions demandées

Inconnues : toolchain/contexte GPU du jour non testés, détail des commandes ASan/shuffle à redécouvrir, reproduction C2/F8 non exécutée, transport des profils non prouvé. Les comptages anciens ne sont pas un verdict courant. Une erreur d'échappement Context Mode a été corrigée ; aucune panne du service constatée.

Décision groupée demandée : confirmer les six branches, dont l'autorisation spécifique B1, et le niveau proportionate avec obligations projet ; accepter les critères ci-dessus ou fournir les corrections importantes. C2 reste ouvert sans acceptation distincte d'une éventuelle fermeture motivée. Les corrections seront persistées sous `verification` avant une nouvelle présentation `/ensemble-mission config`. Le `start` appartient exclusivement à l'utilisateur ; aucun worker ne part sur un simple GO en prose et une pause opérateur exige sa reprise distincte.
