---
title: "Pointeur archivé — mission lifetime OpenGL clôturée"
category: architecture
tags: [artist, architecture, rendering, testing]
aliases: [lifetime-opengl-pointer-archive]
summary: "Archive du pointeur next-mission au moment de sa clôture lifetime. Le pointeur vivant propose ensuite le chemin d'exécution Pipeline."
status: completed
implementation: verified
source_revision: 71dacbb40ad8746fce5e57cd3cbecc2e1fa8a493
created: 2026-10-18
updated: 2026-10-19
sources:
  - "opencode:ses_eacf31fbaffeam2skOnm89Pi66"
  - "opencode:ses_eacf2ea5affeBDZfFmS3E05oRl"
  - "references/session-2026-10-18-attribute-gl.md"
provenance:
  extracted: 0.5
  inferred: 0.5
  ambiguous: 0.0
---

# Mission lifetime OpenGL — clôturée au périmètre

> [!note] Pointeur archivé le 2026-10-19
> Exemplaire du pointeur vivant au moment de la clôture. La proposition suivante
> est [[architecture/next-mission|le pointeur]] et
> [[architecture/missions/2026-10-19-pipeline-execution-path|l'ordre de mission]].

> [!important] Exécutée puis close le 2026-10-19
> Candidat moteur `71dacbb`. Attestation fraîche : **466/466** unitaires et
> **64/64** OpenGL, Windows x64 Debug, binaire unitaire `0a952ff3…`.
> Audit V : PASS borné, registre COVERED. Aucune mission suivante n'est
> sélectionnée. ADR-0003 reste Accepted/partial. Le texte sous cette note est
> le cadrage d'origine, pas l'état ouvert du chantier.
>
> Preuves : [[references/session-2026-10-19-opengl-lifetime|la session]] et
> [[architecture/missions/2026-10-19-opengl-lifetime-audit-v|l'audit V]].

## Origine et état réel

L'[[architecture/missions/2026-10-19-attribute-session-ingestion|ingestion de
session Attribute-GL est archivée]]. Elle n'a pas adopté de garde-fous ni fermé
le chantier moteur. La [[references/session-2026-10-18-attribute-gl|rétrospective]]
conserve les décisions et rectifications, dont le retrait des sondes purement
driver. Ne pas reproposer l'implémentation Attribute-GL déjà intégrée.

Les deux PM ont enquêté en lecture seule sur `070e20d`. Leurs avis ne comportent
aucun nouveau run :326PASS Windows Debug (266CPU+60GL) est une preuve historique
du retrait Q3 àf7bb171, pas la baseline exécutée de cette future mission.
Le pointeur Attribute antérieur, y compris son diff documentaire préexistant,
est conservé intégralement dans Git :
`28395ff16f9026522f593a012396bfe915264971:docs/architecture/next-mission.md`.
Ne pas le lire comme une description actuelle des défauts à corriger.

Dirty AGENTS/profils/harness/index/log/correction-plan/.grepai et politiques
externes préservés. L'autorisation présente porte sur ce pointeur et l'archive,
pas sur ces autres fichiers. Un futur worktree ne reçoit pas le dirty implicitement.

## Résultat attendu et bornage

**Rendre déterministes les frontières de création, remplacement, refus et
libération des ressources OpenGL existantes**, sans reconstruire le moteur.
Quatre branches d'auteur fournissent une enveloppe long run significative :

1. Fermer le cycle des VBO créés pour Attribute, selon un contrat de contexte
   courant et d'ownership explicitement adopté.
2. Définir puis respecter la frontière de publication de la réflexion Pass :
   succès, échec tardif, remplacement, noms disparus et emprunts invalidés.
3. Définir puis respecter les refus Uniform et l'état CPU observable après refus,
   sans inventer une compatibilité GLSL universelle.
4. Requalifier et, si nécessaire, isoler les fixtures Loader/ShaderLoader sous
   permutations, sans masquer les erreurs de production.

La largeur vient de frontières testables et de fichiers disjoints, pas de l'ajout
de fonctionnalités sans oracle. Chaque branche peut aboutir à un constat sans
patch si sa suspicion ne se reproduit pas. Ne pas fabriquer un RED pour remplir
le budget ni présenter la publication partielle historique comme un bug adopté.

### Observations statiques transmises par les PM

- `ARTIST/include/graphic/opengl/pipeline/component/attribute/Binder.hpp` crée et
  publie un VBO ; aucun glDeleteBuffers n'a été trouvé dans ARTIST/include lors
  de leur enquête. Attribute a destructeur/moves par défaut. Ce constat ne décide
  pas encore qui doit supprimer le nom, quand ni sous quel contexte.
- Les Readers `pass/AttributeReader.hpp` et `pass/UniformReader.hpp` publient dans
  leur boucle ; `PassContext::addAttribute/addUniform` remplace immédiatement
  l'owner. Une publication partielle est déjà caractérisée par des tests.
- `Uniform::set` stocke la valeur avant le Setter ; un refus de cardinalité peut
  suivre. Le contrat ne promet actuellement pas partout un rollback CPU. Les
  compatibilités ne se réduisent pas à l'égalité naïve de deux enums.
- Les incidents shuffle Loader/ShaderLoader sont historiques, non reproduits sur
  ce candidat pendant le cadrage. La lecture fichier Shader a déjà été corrigée :
  ne pas la reproposer comme neuve.

Ces constats sont des pistes à contractualiser, **pas des bugs reproduits par
cette préparation**. GrepAI et Serena ont aidé les PM ; une recherche symbolique
Loader vide a été complétée textuellement. La fiabilité exhaustive de leur analyse
C++ sans base de compilation complète n'est pas démontrée.

### Hors périmètre

Nouveaux backends, render graph, assets/cooker, handles générationnels, gestionnaire
global de contexte, retraite GPU générale, RAII exhaustive, refonte de Pass::load
implicitement transactionnel, support générique des types, perf/zero-cost, image
de référence, portage OS et réparation générale du harness agentique.
Pas de glFinish par Attribute pour masquer un contrat de complétion absent.
Le nettoyage Shader avant publication, autre suggestion PM, reste une tranche
distincte à préparer seulement si un RED causal est établi ; il n'est pas ajouté
comme cinquième branche pour gonfler artificiellement le parallélisme.

## K — Décisions à fermer avant auteurs

Principal `engine-architect` ; renfort ressources seulement pour le ledger natif.
Le spécialiste OpenGL reste `gpu-engineer` pour l'implémentation du backend.

1. **VBO :** owner/adoption/emprunt de setBufferID, rebind, release explicite ou
   automatique, exact-once, move chargé→chargé, destruction/remplacement en map,
   état moved-from, contexte/thread et cas sans contexte. Références VAO et
   suppression logique ne prouvent pas une retraite physique GPU générale.
2. **Réflexion :** atomicité par Reader ou Pass complet, ancien état conservé ou
   effacé en échec, collection vide/noms disparus et moment d'invalidation des
   emprunts. Tout changement de garantie publique requiert décision et oracle.
3. **Uniform :** état CPU après rejet, matrice CPU/GLSL admise (bool/samplers/
   entiers inclus), programme courant et doubles/extensions. Aucun élargissement
   ni retrait silencieux du contrat existant.
4. **Interactions :** responsabilité de PassContext lors du remplacement d'un
   Attribute désormais owner VBO ; interfaces de métadonnées stables entre B/C.
   Séparer les fichiers auteur, ou sérialiser la frontière si elle ne se ferme pas.
5. **Dette Attribute-GL :** relever l'audit final encore ouvert ; établir avant
   mutation un candidat et une baseline courants. Tout FAIL causal dans les mêmes
   frontières est traité ou arbitré avant consommation, jamais absorbé par326PASS.

Le lead persiste les options et la décision humaine préalable sur ces garanties.
Si elles imposent une architecture globale, K s'arrête et rend le problème au
lead : long_run n'autorise pas une refonte silencieuse ni une décision produit.

### Binding times

Traits/layouts/dispatch connus par les types : compile-time. Réflexion et
validation du programme réellement lié : préparation runtime après link GLSL.
Identités GL, contexte/thread, valeurs, borrows et transitions d'owner : runtime.
Aucun nouveau contrat cook ou link-time C++ n'est présumé.

## Graphe exécutable à préparer, non publié

Évaluation PM/LLM : **C brute→retenue / R** sur100, heuristiques indépendantes.
Développement, code de tests et harness : C retenue=max(70,brute). Risque jamais
modifié pour obtenir un modèle. Contexte fresh pour chaque fiche ; lecteurs RO
sans worktree, auteurs isolés. Aucun modèle/provider/variante imposé par ce plan.
Les références ci-dessous ne sont pas des IDs Ensemble.

- **P — entrée documentaire**, lead, **30→30/35**. Archive réalisée et pointeur
  proposé ; pas de scope moteur commencé. Docs canoniques hors ownership restent
  intactes. Le HEAD intégré sera transmis à K/Q.
- **K — contrats**, engine-architect RO, **76→76/78**. Ledger, états, erreurs,
  alternatives et décisions ci-dessus, sans source modifiée. Puis validation lead/
  humaine. Un contrat incompatible avec l'autonomie bloque les auteurs.
- **Q — plan QA**, qa-planner RO, après K validé, **60→60/62**. Exigences
  explicites/inférées, nominal/négatif, invariants, top3 risques, preuve↔obligation
  et orphelins ; QA I×P(1–5), champs déclencheur/blast radius/détection/mitigation/
  gap pour tout produit≥10. Lead persiste et commit avant TOUT auteur.
- **M — éventuelles entrées mock communes**, cpp-engineer, **45→70/58**
  (provision lead pour résoudre un conflit d'ownership détecté entre les plans).
  Seulement après K/Q committés ; propriétaire unique de
  `ARTIST/tests/mock/opengl/glFunctionMock.hpp` si une extension est nécessaire.
  RED/GREEN local au double, comportements historiques préservés ; pas le RED
  métier des auteurs. Borner/réévaluer cette fiche après K ; si inutile, pas de
  tâche factice. Intégrer M avant ses consommateurs, pas en parallèle avec eux.
- **A — cycle VBO**, gpu-engineer, **82→82/86**. Seul propriétaire de
  `graphic/pipeline/Attribute.hpp`, contexte Attribute, composants `attribute/`,
  tests Attribute associés et nouveau fichier GL lifetime. Pas de PassContext,
  Uniform, Readers ou mock commun. RED exact-once/move/rebind/release selon K,
  GREEN minimal et régression des12formes/Pointer/IPointer/VBO propre.
- **B — publication réflexion**, cpp-engineer distinct, **78→78/80**. Seul
  propriétaire de `context/PassContext.hpp`, Readers `pass/AttributeReader.hpp`
  et `pass/UniformReader.hpp`, tests Readers et fichier d'intégration dédié.
  Pas de contexte Attribute/Uniform ni de mock commun. RED première entrée valide
  puis seconde invalide avec ancien état identifiable ; succès/échec/vide/noms
  disparus selon K. Ne pas affaiblir le test de publication historique : tout
  changement est un amendement explicite de contrat.
- **C — refus Uniform**, autre cpp-engineer, **68→70/72**. Seul propriétaire de
  `pipeline/Uniform.hpp`, contexte Uniform, `uniform/Setter.hpp`, tests de valeurs/
  cardinalité/compatibilité et nouveau fichier GL dédié. Pas de Reader ni de mock
  commun. RED via API publique, ancienne valeur observée selon la garantie K,
  zéro upload au refus ; succès par famille réellement autorisée.
- **H — isolation fixtures**, build-engineer, **56→70/55**. Seulement les deux
  `ARTIST/tests/src/graphic/opengl/pipeline/component/pass/LoaderTests.cpp` et
  `ARTIST/tests/src/graphic/opengl/pipeline/component/shader/LoaderTests.cpp`,
  et helper neuf local si nécessaire. Chemins confirmés sur disque pendant ce
  cadrage. Pas de Loader production, CMake ou
  mock global. Seeds historiques et permutations inversées sur candidat identifié,
  contrôle négatif conservé ; sinon caractérisation sans patch artificiel.
- **I — intégration et interactions**, lead avec propriétaires, **64→70/76**.
  A/B/C/H réellement shutdown/merge, diff relu puis tests croisés remplacement
  VBO/réflexion et Uniform réfléchi/upload. Tout code de résolution retourne à son
  owner ; toute nouvelle fixture ou adaptation reçoit ownership et tranche TDD.
- **G — gaps**, test-engineer RO, après I, **58→58/60**. Propositions texte avec
  oracles et KEEP/MODIFY/REJECT/DEFER ; pas de production ni runner autonome.
  Il complète les auteurs, ne fournit pas leur RED. Le lead persiste et exécute.
- **F — correction des gaps acceptés**, provision **62→70/65** pour code de test.
  Ne pas publier une tâche générique : chemins, owner et scores réévalués sur les
  vrais gaps de G ; correction production renvoyée à A/B/C/H. Nouvelle intégration.
- **R — revue lifetime**, correctness-reviewer fresh RO, après I,
  **66→66/78** (estimation PM indépendant). Parallèle à G ; findings corrigés par
  propriétaires, invalidation des conclusions affectées après mutation.
- **V — audit final**, test-reviewer fresh HORS board d'implémentation,
  **70→70/77**. Candidat final intégré, contrat et preuves seulement, pas les
  justifications auteur. Scénario discriminant avant finding haut, puis retour
  propriétaire/réintégration/ré-audit. Rendering-verifier en renfort si une preuve
  GPU reste ambiguë ; pas de nouveau coordinateur.

```text
P → K → décisions → Q → commit contrats/QA → [M intégré si nécessaire]
                                           ↓
                                     A || B || C || H
                                           ↓
                                   I → G || R → corrections bornées
                                           ↓
                                  candidat final intégré → V fresh
                                           ↓
                                  validation lead → documentation
```

La rédaction P est indépendante des investigations K. A/B/C/H sont parallèles
seulement après gel des interfaces et intégration des entrées communes. Si le
contrat révèle un fichier commun restant, un seul owner et une nouvelle barrière.
Le lead garde les consommateurs hors board jusqu'aux vraies intégrations ; pas
de tâche factice d'intégration, pas de dépendance sur la simple fin d'auteur.

## Exécution autonome et ressources

Une fois les contrats, QA et politique approuvés, le coordinateur peut enchaîner
les tâches prêtes, corrections bornées et validations sans redemander chaque
détail local. Escalade obligatoire pour contrat/portée ouverts, accès manquant,
préférence modèle différente, FAIL critique ou deux essais sans information
nouvelle. Healer QA au plus deux réparations ; bug production reste rouge.
Une pause opérateur exige son GO distinct ; long_run n'autorise pas à la lever.

Un écrivain par fichier. Les écritures A/B/C/H sont parallèles ; les builds sont
sérialisés par arbre de sortie, même si les auteurs sont indépendants. Un seul
bail GPU/campagne GL ; processus partageant fixtures/singleton mock sérialisés.
Un indexeur QMD canonique, lead runner pour les propositions QA. Pas de mélange
des preuves de branches ni d'arrêt d'une ressource externe au lot.

## Preuves et conditions de sortie

Avant tests, charger artist-build-validation et vérifier les préconditions réelles.
Commandes futures à adapter au build identifié, non exécutées pour ce cadrage :

```text
scripts/build-windows.ps1 -Configuration Debug -TestLabel unit -Jobs 1
ctest --test-dir <build> -C Debug -N --show-only=json-v1
ctest --test-dir <build> -C Debug -L unit --no-tests=error --output-on-failure --parallel 1
ctest --test-dir <build> -C Debug -L opengl --no-tests=error --output-on-failure --parallel 1
```

Reconfigurer après ajout de sources, vérifier découverte non nulle et nouveaux
tests présents ; shuffle sur le binaire unit identifié. ASan seulement après
préflight, arbre séparé, canari et sélection documentés ; absence de preuve reste
INCONCLUSIVE, sans héritage du résultat historique. Ne pas supprimer une erreur
de production en corrigeant une fixture ni ajouter de sonde de conformité driver.

RED causal de chaque branche avant implémentation, GREEN minimal puis refactor/
non-régression ; environnement indisponible et zéro test ne sont jamais RED/PASS.
Mocks prouvent arguments/absence d'appels ; GL réel prouve les obligations natives
retenues. glIsBuffer seul ne prouve ni ownership exact-once ni retraite physique.
Tous les cas création/release/move/remplacement, échec tardif de réflexion et refus
Uniform du contrat K/Q ont un verdict attribué au candidat final. Rapport : HEAD,
delta/hashes, commandes, environnement, comptages, skips et limites. Aucun PASS
global tant qu'un critère requis est FAIL ou INCONCLUSIVE.

### Budget proposé, pas durée promise

PM principal : setup5–9 h-personnes ; travail productif32–50 h-personnes ;
buffer11–17 h supplémentaires, soit environ75% planifié/25% buffer après setup.
Ces nombres ne sont ni un SLA ni une durée murale ; les runs GPU/build et le
chemin K→Q→A/B→I limitent le gain du parallélisme. Réévaluer après K et M ;
ne pas consommer le buffer pour élargir la portée. Couper le confort avant un
oracle haut risque et ne jamais financer une branche sans cause établie.

## Adoption et démarrage futurs

Le lead retient ce candidat large du PM principal. L'avis indépendant privilégiait
un lot VBO+nettoyage Shader plus étroit ; sa réserve est conservée : si K ne peut
fermer réflexion/Uniform sans refonte globale, réduire au cycle VBO et rendre les
autres garanties au décideur, plutôt que poursuivre implicitement.

La présente demande autorise **la préparation documentaire seulement**. Préparer
une nouvelle mission native avec cet objectif, puis demander à l'utilisateur :
`/ensemble-mission config --mission-mode long_run`, présentation config de la
politique complète et `/ensemble-mission start`. Ne pas invoquer start à sa place,
réutiliser l'approbation de l'ingestion ou choisir des modèles hors grille.
Avant publication future : require_context, deux scores, affinités et vrais IDs
des producteurs intégrés ; aucune classification JEV sans choix explicite.

ADR0003/0005 Accepted/partial,0008 Windows Debug seulement ;0001/2/4/6/7 Proposed.
K doit adopter explicitement toute garantie nouvelle sans promouvoir les ADR
globaux par accident. Voir [[adr/README|le registre]] et
[[architecture/correction-plan|l'ordre de correction]], dont le pointeur Attribute
antérieur reste un état documentaire historique, non corrigé dans un dirty externe.
