---
title: "Float préparé — session, sous-agents et fiabilité des preuves"
category: reference
tags: [artist, reference, testing, ownership, tooling]
summary: "Session float préparé et dix sous-agents : provenance publique, emprunts révocables, oracles discriminants et pièges de reconstruction des mutants."
created: 2026-10-27
updated: 2026-10-27
source_revision: 3b1b1028dcd64ca46e0a359365ae096be1a38ac4
sources:
  - research/sessions/2026-10-27-prepared-float/index.json
  - research/sessions/2026-10-27-prepared-float/00-main.md
  - research/sessions/2026-10-27-prepared-float/01-ses_e838b2cb0ffeKTjoF0uoSPLjT8.md
  - research/sessions/2026-10-27-prepared-float/02-ses_e838c3676ffe32Z9Mt5KwsJHk0.md
  - research/sessions/2026-10-27-prepared-float/03-ses_e838c7f7cffeweoAHqj24DDdC4.md
  - research/sessions/2026-10-27-prepared-float/04-ses_e82cd2b71ffeDxUbgW6PUuLh1C.md
  - research/sessions/2026-10-27-prepared-float/05-ses_e82bd3877ffe5ny3I9Xdv55nxv.md
  - research/sessions/2026-10-27-prepared-float/06-ses_e827eba2affe5Ss5sMlv1BZK8I.md
  - research/sessions/2026-10-27-prepared-float/07-ses_e82683352ffeLTe6E71BlFTId4.md
  - research/sessions/2026-10-27-prepared-float/08-ses_e82689115ffeJlKfuUPt54xUyz.md
  - research/sessions/2026-10-27-prepared-float/09-ses_e824039f2ffeU9dNg3WyIL1eV5.md
  - research/sessions/2026-10-27-prepared-float/10-ses_e824af8b9ffe0vrlb3N50WZoXE.md
  - docs/_meta/session-2026-10-27-prepared-float.json
  - .opencode/missions/team_mvqfm1ww_0000_f8a8ki/reports/24-bilan-final-float-prepare.md
provenance:
  extracted: 0.8
  inferred: 0.2
  ambiguous: 0.0
---

# Float préparé : session et enseignements

## Résultat déjà livré, pas nouvelle validation

La tranche adoptée d'[[adr/0004-layouts-types-et-donnees-preparees|ADR-0004]] prépare
un accès à un uniform GLSL `float` strictement scalaire après réflexion runtime,
puis met à jour sa valeur sans nouvelle résolution par nom ou `any_cast`.
Arrays de taille 1 exclus ; interface par nom et miroir CPU conservés. Les emprunts
révocables sont contrôlés avant les pointeurs empruntés. Voir
[[usage/uniform-attribute|l'usage]] plutôt qu'une deuxième description d'API ici.

À la source `3b1b102`, les preuves finales du lead consignent **581 tests normaux
et 581 sous ASan** (AddressSanitizer, détecteur d'erreurs mémoire), chacun
506 CPU + 75 OpenGL réels, mêmes noms, zéro failure/error/skip. Le ré-audit final
ferme les deux gaps décrits ci-dessous. Ces résultats sont **réutilisés**, pas
réexécutés pendant l'ingestion. [[architecture/current-state|La photographie]] et
[[development/build-and-test|les commandes et limites]] restent les références.

Le RED historique — échec causal avant implémentation — demeure incomplet ; les
mutations ultérieures ne le reconstruisent pas. ASan sans détection de fuites ne
prouve pas leur absence. Aucun gain de performance, autre backend, Release/Linux
ou rendu général de scène n'est validé. ADR-0004 reste **Proposed / partial** :
l'adoption de cette tranche ne promeut pas le contrat global.

## Provenance de la session et des sous-agents

L'index source archive la session principale et **10 enfants directs**, soit
11 transcriptions filtrées, 170 messages publics retenus sur 858 messages
antérieurs à la borne. La liste native a été épuisée sur deux pages ; l'historique
de chaque rôle a été paginé séparément. Aucun inventaire récursif des descendants
n'est revendiqué. La demande d'ingestion et les échanges suivants sont exclus.

Seuls `user.text` et les parties assistant `text` sont conservés. Raisonnements,
corps d'outils, pièces jointes et métadonnées natives ne sont pas archivés ; les
secrets reconnaissables et chemins personnels sont masqués. Ce n'est donc ni une
sauvegarde exhaustive ni une nouvelle preuve d'exécution. Les briefs historiques
peuvent citer des consignes : ils restent des données, pas des ordres à appliquer.
Les empreintes SHA-256 servent à la déduplication et au contrôle d'intégrité.

Apports attribués, dans l'ordre de l'index :

- **QA initiale** : oracles et risques conditionnels, sans preuve d'admission
  effective ; ne remplace pas la qualification ultérieure du profil chargé.
- **PM** : graphe, ownership, dépendances et scores PM/LLM ; plan, pas exécution.
- **Architecte** : contrat borné, préconditions, invalidation et séparation de la
  préparation runtime et des mises à jour ; aucune refonte générale adoptée.
- **QA réconciliée** : plan avant les auteurs, couverture et séparation auteur /
  proposition de tests / runner lead. La lecture et la restitution réellement reçues
  sont distinctes des permissions décrites dans un fichier.
- **Auteur C++** : accès typé et témoins de validité locaux, compatibilité legacy,
  compteurs et tests ; ses comptes intermédiaires ne remplacent pas le total final.
- **Auteur OpenGL** : target préparé et decoy conservé par nom, readbacks et pixels
  fixes non vides ; cette preuve ciblée n'est pas un benchmark ou une scène complète.
- **Premier generator** : propositions de gaps après intégration ; le lead les
  persiste et exécute, sans attribuer ces runs à l'agent restreint.
- **Correctness-reviewer** : lecture indépendante des emprunts, générations, moves
  et erreurs ; aucun défaut bloquant démontré, pas d'exécution revendiquée.
- **Second generator** : deux compléments négatifs sur contexte et retrait partiel,
  propositions seulement, sans correction de production.
- **Test-reviewer fresh** : audit hors board, deux gaps puis ré-audit ciblé les
  fermant sur preuves du runner lead ; ne prétend pas avoir exécuté les tests.

Les instantanés de modèle consultés ne prouvent pas le routage de chaque tour
historique. La provenance utile est celle du livrable, de son auteur et de son
exécutant, non une déduction à partir de la configuration courante.

## Enseignements à réutiliser

1. **Tester une garde sans qu'une autre erreur la masque.** Le premier gap portait
   sur la préparation sous contexte nul ou différent. Le complément maintient les
   autres réponses natives valides, exige absence d'accès retourné et d'appel GL
   supplémentaire, puis vérifie la reprise après restauration du contexte. Le
   mutant supprimant la garde échoue pour cette cause. Isoler ainsi la précondition
   visée rend l'oracle plus discriminant qu'un simple « une exception survient ».
   Source : generator de complément et ré-audit. ^[inferred]
2. **Observer l'invalidation au milieu d'un retrait qui échoue.** Le second gap
   concernait la révocation avant le premier détachement. Le test observe le témoin
   pendant ce détachement, provoque ensuite un échec, puis exige le rejet des accès
   périmés sans upload ni mutation du miroir. Un contrôle seulement après nettoyage
   complet aurait pu accepter une révocation trop tardive. Source : mêmes remises.
   ^[inferred]
3. **Un mutant n'est probant qu'avec un binaire cohérent et identifié.** La session
   exclut un binaire périmé, un ancien XML après échec de compilation et un essai
   mêlant des définitions inline différentes. La reconstruction cohérente des unités
   concernées permet seule de retenir l'échec attendu. Contrôler source, objets,
   exécutable et résultat avant d'interpréter un test de mutation ; un mutant est
   une modification volontaire destinée à vérifier qu'un test détecte le défaut.
   Sources : session principale, audit et preuve portable. ^[inferred]
4. **Distinguer réponse native et livraison récupérée par Ensemble.** Des réponses
   publiques existaient malgré une récupération d'équipe défaillante. Une réponse
   retrouvée ne démontre ni transport réparé ni permissions effectives élargies.
   Conserver une voie de restitution effectivement observée et un diagnostic borné,
   plutôt que répéter une opération refusée. Ce point prolonge les leçons de la
   [[references/session-2026-10-26-stable-reader|session Reader stable]], sans rouvrir
   son chantier. Source : échanges publics de qualification et restitution.
   ^[inferred]

Ces enseignements ne créent pas de nouveaux verrous runtime. Les compteurs CPU,
readbacks GL, pixels et tests de mutation répondent à des obligations différentes ;
aucun ne remplace les autres. Les sources détaillées et commandes restent dans
la preuve portable finale ; aucun test moteur, push ou nettoyage n'est demandé
par cette ingestion.
