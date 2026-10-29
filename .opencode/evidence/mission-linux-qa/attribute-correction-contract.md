# Correction Attribute Linux — contrat borné adopté par le lead

Date : 2026-10-28. Mandat : correction compatible nécessaire à la parité QA Windows/Linux, sans nouvelle API ou nouvelle provenance de données. Proposition : architecte pool-7, tâche `task_mvsfp3ei_001o_j80gqi`, lecture seule. Adoption du lead limitée au présent contrat, pas aux ADR globaux.

## État et preuve de départ

Code exécutable de départ `a4c0f82`. HEAD lead ultérieur `7ef9e9a` contient seulement des ajouts documentaires depuis cette base. Diff canonique préexistant de profils/harness/docs hors ownership. Workflow/helpers pool-5 non intégrés, ownership distinct et aucun fichier C++ modifié par eux.

Linux normal et ASan : 506CPU réussis et72/75OpenGL, trois failures identiques sans skip. Windows au candidat indiqué : 581/581 normal et581/581ASan. Les trois échecs initiaux restent dans les archives et ne constituent pas seuls un RED causal du nouveau comportement de classification.

Sonde exécutée Mesa25.2.8/llvmpipe, contexte4.5core obtenu et3.3demandé, extension annoncée et entrées modernes chargées : mêmes noms legacy et localisations base/[0] pour scalaire et array1. La requête de localisation `[0]` ne discrimine pas. La réflexion moderne distingue nomscalaire et nomarray[0]. Preuves copiées/recoupées sous `out/logs/mission-linux-qa/attribute-name-probe/` ; pas de draw/upload ni corruptionnative démontrée.

## Invariants adoptés

1. Conserver l'énumération legacy, **chaque clé native legacy exacte**, les maps/getAttribute et tous champs/interfaces publics. Aucun alias ou canonicalisation publique.
2. Utiliser le nom moderne seulement comme information temporaire de classification dans le Reader, lors de la réflexion du même programme lié. Pas de lookup supplémentaire au set/draw/frame, pas de parseur GLSL, metadata cook, dépendance nouvelle ou capability-manager global.
3. Route moderne seulement si version effective>=4.3 **ou** extension effective GL_ARB_program_interface_query, **et** toutes les fonctions utilisées sont chargées. Réutiliser GLEW et les patterns existants ; pointeur ou token seul ne suffit pas.
4. Associer l'entrée legacy à PROGRAM_INPUT du même programme par son nom natif, pas par l'ordre d'une map ni la seule location. Option locale admise : GetProgramResourceIndex puis propriétés NAME_LENGTH/TYPE/ARRAY_SIZE/LOCATION et GetProgramResourceName. Valider index, bornes, longueur/NUL, correspondance du nom (exact ou suffixe unique `[0]` ajouté à la base native), type/taille/location concordants. Pas de traitement de `[00]`, `[1]` ou suffixe arbitraire comme `[0]`.
5. Si capacité/entrée manque : zéro appel moderne et chemin legacy antérieur. Si route moderne disponible mais réponse incohérente/malformée : échec explicite du staging, **pas de repli silencieux en scalaire ou fallback**. Le diagnostic précis sera figé dans les tests avant code.
6. Nom moderne validé détermine isGLArray, même size1 ; ARRAY_SIZE1 seul ne discrimine pas. Scalaire admissible au Setter typé ; arrays1/2 refusés avant mutationnativeGL et du buffer leurre.
7. Préserver staging/replaceAttributes : map et pointees publiés inchangés en cas d'échec ; nouveaux emprunts publiés seulement au commit. Programme lié et contexte approprié courants, pas de relink/destruction concurrent. Les IDs/locations ne sont pas des générations stables.
8. Pas de refonte du Setter, du wrapper IAttribute::set, de son stockage ou d'Uniform. Le wrapper stage déjà la valeurCPU avant le Setter : la conservation de l'ancienne valeurCPU après rejet n'est **pas** une garantie ajoutée. Les tests directs du Setter peuvent préserver leur miroir préinstallé selon leur propre frontière ; ne pas confondre les deux.

## Qualification et limite3.3

Correction à vérifier sur les configurations réellement disponibles : Windows/NVIDIA et Linux/Mesa moderne. Le chemin3.3 historique reste inchangé si la capacité moderne manque. Une voie négative mockée doit le prouver, car un contexte3.3 NVIDIA peut annoncer l'extension.

Sans information moderne, size1+nomnu ne distingue pas universellement scalaire/array1 ; le fallback ne devient donc pas une preuve universelle de scalarité. Un autre pilote3.3 non étendu à nom ambigu reste un **gap explicite de qualification**. Si ce pilote devient cible requise, le gap sera une obligation non satisfaite à disposer, jamais un skip/PASS. Aucune nouvelle politique publique de rejet/support ni exigence4.3globale n'est adoptée.

## Tranche auteur prévue, après plan QA complémentaire commité

Ownership cible : Reader OpenGL, tests CPU/mocks Reader strictement requis, tests composants Attribute. Un propriétaire pour RED→GREEN→REFACTOR. Fichiers exacts des mocks à réserver avant édition ; pas de conflit avec workflow/helpers pool-5. Aucun changement de source autorisé avant cette barrière.

RED nécessaire : récupérer indépendamment le nomlegacy réel du programme Mesa, vérifier publication sous cette seule clé puis exiger isGLArraytrue pour array1 ; l'échec doit porter sur la classification, pas sur une orthographe vendor, une toolchain ou un contexte absent. Témoins scalaire identique type/size/location et array2. Conserver les anciens résultats rouges et leur disposition explicite ; ne pas les effacer ni prétendre avoir réécrit leur historique.

Tests graphiques cibles : clés exactement identiques aux noms natifs indépendamment obtenus du même programme, pas de nouvel alias ; size/type/location exacts, scalarfalse avec upload admis, array1/2true avec ARRAY_NOT_SUPPORTED avant mutationGL/binding/leurre. Conserver les oracles existants de rendu/transformfeedback/pixels sans élargissement de tolérance. Le préflight doit mesurer des préconditions utiles, pas réintroduire un test bloquant de conformité fournisseur sans consommation moteur.

CPU/mocks ciblés : gatecore4.3 ; gate3.3+extension ;3.3sans extension malgré pointeursprésents ; chaque entrée manquante ; index absent, longueur/NUL invalides, noms incohérents, type/taille/location contradictoires. Anciennes map/pointees conservées sur erreurs tardives. Préparer explicitement capacités et pointeurs, jamais appelGLEW réel involontaire (pattern fp64 du mock existant à examiner).

GREEN : recompilation des TU touchées (header-only), inventaires et JUnit au candidat intégré, normal/ASan selon le planQA, résultats Windows et Mesa distingués. Pas de gain de performance revendiqué ; coût de réflexion amorti par génération, pas d'appelnouveauauupdate à vérifier selon contrats disponibles. Tests, erreurs et rollback restent aux frontières effectivement définies.

## Graphes, responsabilités et évaluations

1. Avis architecte final → présent contrat persisté/commité par lead.
2. Complément QA en lecture seule sur ce contrat et les risques de gates/staging → lead persiste et commite avant worktree auteur.
3. Auteur OpenGL unique → RED causal, correction minimale, tests/gates/staging, GREEN sur son candidat ; propriétaireGPU Linux coordonné avec fin de campagne pool-5.
4. Shutdown/merge et relecture lead → consommateur de gaps/revue indépendante appropriée sur candidat réellement intégré, puis finalisation QA/documentaire. Les tâches consommatrices restent différées avant intégration.

Parallèle disponible : workflow/helpers et CI de pool-5 sans ownership C++, planQA en lecture seule, aucune compilation concurrente d'un même arbre ni compétition pour GPUlogiciel. Avis et plan ne dépendent pas du helper non intégré.

Évaluation locale PM/LLM de la future tranche de développement : complexité brute84/retenue84, risque76 ; incertitudes fonctionsGLEWmocks et validationstaging. Ce sont des scores de tâche, pas probabilités. PlanQA complémentaire non-développement : complexité55/risque72. Politique native approuvée inchangée, aucun modèle explicite. QA I×P sera distinct des scoresPM.

## Statuts et limites de preuve

ADR0003/0005 Accepted/partial ;0001/2/4/6/7Proposed ;0008WindowsDebugseulement, aucune adoptionnouvelle. AvisarchitecteRO, aucune sourcemodifiée ni test/GPU exécuté par lui. SerenaARTIST confirmé, couverturetemplate/références limitée et complétée par lecturesciblées. QMDabsent de son catalogue et politiqueexterne refuséeunefois ; lead fournit extraits/copie locale sans élargissement. Une copie disponible ne prouve pas sa lecture ni un enforcementexhaustif.
