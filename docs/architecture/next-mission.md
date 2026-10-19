---
title: "Prochaine mission — contrat Attribute réfléchi OpenGL"
category: architecture
tags: [artist, architecture, rendering, testing]
aliases: [next-mission]
summary: "Périmètre proposé après F7 : chaîne Attribute réfléchie OpenGL, 12 formes CPU canoniques, validation avant mutation GL et VBO correctement ciblé ; lifetime VBO séparé."
status: proposed
implementation: not-started
source_revision: 4a9b722a8f1b64470172686aee961221fa6384c0
created: 2026-10-18
updated: 2026-10-18
---

# Prochaine mission — contrat Attribute réfléchi OpenGL

> [!important] Préparation seulement
> Cadrage architecte/PM intégré. Aucune tâche d'implémentation publiée, aucun test
> RED exécuté, aucune nouvelle politique de modèles approuvée. Le périmètre
> recommandé ci-dessous est prêt pour préparation native, pas pour exécution.

## État de départ et sauvegarde

L'ancien pointeur est conservé **à l'identique** dans
[[architecture/missions/2026-10-18-next-mission-f7-closed|la sauvegarde F7 clôturée]].
Son propriétaire est le coordinateur ; ce fichier non suivi n'est pas un travail
inconnu d'un auteur. La [[architecture/missions/2026-10-18-f7-uniform-attribute|mission F7]]
et la [[references/session-2026-10-18-f7|session avec ses 28 sous-agents]] restent
les archives techniques et opérationnelles.

HEAD inspecté : `4a9b722`. Code moteur inchangé depuis `6d6dcb7`, candidat des
**299/299 tests natifs Windows Debug** (251 CPU + 48 OpenGL). Ces résultats sont
historiques : aucune nouvelle campagne exécutée pour ce cadrage. ASan 245 reste
l'historique T4, pas une preuve ASan finale. `.grepai/config.yaml` est hors périmètre.

## Pourquoi cette tranche

Constats statiques, **pas encore des bugs reproduits** :

- `ARTIST/include/graphic/opengl/pipeline/component/pass/AttributeReader.hpp`
  conserve directement la taille et le type retournés par `glGetActiveAttrib`.
- `ARTIST/include/graphic/opengl/pipeline/component/attribute/Setter.hpp`
  utilise les métadonnées du contexte pour `glVertexAttribPointer`.
- La cardinalité réfléchie d'une variable GLSL n'est pas le nombre de composantes
  d'un élément mémoire ; un enum GLSL vectoriel n'est pas un type scalaire GL.
- Les tests GL `ARTIST/component-tests/src/graphic/pipeline/AttributeComponentTests.cpp`
  préparent ces champs manuellement. Leur succès ne démontre pas la chaîne issue
  de la réflexion d'un shader lié.
- Le Binder crée un VBO ; aucun `glDeleteBuffers` n'a été trouvé dans
  `ARTIST/include`. Cela ouvre une question de lifetime distincte, sans établir
  à lui seul le propriétaire ni le point correct de suppression.

Ces écarts concernent la correction du chemin CPU/driver avant une optimisation.
La tranche s'inscrit dans l'étape 1 du
[[architecture/correction-plan|plan de correction]], pas dans une adoption globale
des layouts préparés d'ADR-0004.

## Périmètre recommandé — Attribute-GL

Une seule chaîne verticale : **réflexion du programme lié → description validée
de l'attribut → configuration OpenGL → preuve native**.

1. Séparer explicitement les informations réfléchies GLSL et les paramètres
   décrivant la mémoire CPU. Ne pas transporter un champ ambigu jusqu'au driver.
2. Supporter une famille fermée de 12 formes CPU canoniques : `float`, entier
   signé et non signé 32 bits, chacun scalaire ou vecteur de 2 à 4 composantes.
   Vérifier la correspondance exacte entre valeur C++ et interface GLSL.
3. Définir les erreurs nominales/négatives et leur moment de validation. Une entrée
   refusée ne doit pas produire une configuration GL invalide.
4. Conserver l'ownership unique et les emprunts F7, ainsi que le comportement de
   freeze hors modification expressément justifiée par le contrat.
5. Ajouter une preuve de bout en bout avec un vrai programme lié, pas seulement
   des métadonnées préparées à la main ou un mock des arguments GL.

### Contrat de données et d'exécution

- **Réflexion** : conserver type GLSL et cardinalité séparément ; location signée
  validée avant conversion, `-1` refusé. Les noms doivent être restitués exactement,
  ou refusés sans publication tronquée ; le buffer fixe de 256 octets ne constitue
  pas une garantie. La réflexion décrit le programme réellement lié.
- **Mémoire CPU** : traits fermés de `T` donnant composantes, scalaire GL et stride
  vérifiés sur le layout. La liste canonique des alias GLM par défaut est figée
  dans le contrat QA avant auteur ; variantes alignées/qualifiers alternatives
  exclues tant que leur layout n'est pas prouvé. Ne pas accepter tout `T` arbitraire.
- **Appels** : float → `glVertexAttribPointer` avec normalisation désactivée ;
  entier signé/non signé → `glVertexAttribIPointer`. L'enum GLSL réfléchi ne passe
  jamais directement comme type scalaire. La cardinalité shader n'est jamais le
  nombre de composantes mémoire. Exemple : `(1, GL_FLOAT_VEC3)` → `(3, GL_FLOAT)`.
- **Refus explicites** : matrices, arrays d'attributs, doubles, conteneurs,
  structures arbitraires, conversions implicites/normalisées et mismatch CPU/GLSL.
  Les tableaux réels restent refusés même si leur cardinalité active vaut 1 ;
  l'oracle ne doit pas supposer que `size > 1` suffit à les identifier.
- **Moment du refus** : un type C++ hors famille est rejeté au compile-time si le
  contrat public le permet ; incompatibilité avec réflexion/type shader non
  supporté refusée avant upload/configuration. Le Reader conserve la description
  des formes shader non supportées ; on ne rejette pas le Pass entier pour cela.
- **État initial** : ID buffer nul, métadonnées invalides tant que non préparées,
  usage déterministe et documenté. Pas de lecture de membres indéterminés.
- **Cible d'upload** : `set` cible explicitement le VBO enregistré par l'Attribute,
  même si un autre VBO est lié depuis `bind`. Toutes les validations précèdent
  cette mutation GL ; après succès, `GL_ARRAY_BUFFER` désigne le VBO cible. Pas de
  gestionnaire global d'état ni de restauration implicite promise.
- **Préconditions** : contexte OpenGL approprié et VAO valide courants, VBO cible
  vivant. La preuve native doit respecter ces préconditions. Aucun lifetime du nom
  natif ni emprunt après remplacement/relink n'est prolongé par cette correction.
- **Stockage CPU** : `shared_ptr<T>` et freeze existants conservés ; la garantie
  « pas de mutation GL au refus » n'invente pas de rollback transactionnel du
  payload CPU déjà stocké par l'API. Toute modification de ce comportement exige
  amendement et oracle dédiés.

La proposition float seule a été écartée pour le cadrage : elle casserait les
usages entiers existants au lieu de leur donner la voie GL correcte. Les choix
ci-dessus sont soumis à l'approbation de la future mission, pas adoptés par ADR.

### Binding times

Traits/types/layout et sélection Pointer/IPointer au compile-time ; pas de nouvelle
obligation C++ link-time. Réflexion et validation d'interface à la préparation
runtime après link GLSL (distinct de l'édition de liens C++). VAO, buffer courant
et données à uploader restent runtime. Aucun format cook ni asset dans un type.

### Hors périmètre

- Correction lifetime VBO : investigation indépendante possible, pas de destructor
  GL ajouté sans contrat de contexte courant, propriétaire et références VAO.
- Support générique matrices/arrays/doubles ou variantes mémoire non prouvées ;
  nouveaux backends, GL avancé ou refonte de tous les layouts/handles.
- Reload atomique, handles générationnels, retirement GPU en vol, RAII exhaustive.
- Gate fp64 Uniform sur appareils incompatibles, bugs Shuffle Loader/ShaderLoader.
- Optimisation, benchmark, image de référence, Release et portage multi-OS.
- Réparation générale du harness QA. La capacité de restitution reste toutefois
  une précondition de départ, jamais une hypothèse tirée du nom d'un profil.

### Suite distincte à préparer après cette chaîne

Le VBO est le prochain sujet local de lifetime, pas un correctif concurrent.
Son cadrage devra décider owner du nom natif, adoption/emprunt, release idempotent,
move chargé→chargé et remplacement en map, contexte courant pendant teardown,
références VAO et limites de suppression physique GPU. Si le teardown courant
n'est pas garanti, concevoir la destruction différée séparément ; aucun `glFinish`
par Attribute ni destructor GL improvisé. Les mêmes contextes/fixtures imposent
une intégration séquentielle, sans bloquer la présente correction de données.

## Graphe d'exécution proposé — non publié

Évaluation **PM/LLM**, scores sur 100, validés par le coordinateur. La complexité
retenue de tout développement est `max(70, brute)` ; le risque reste distinct,
jamais une probabilité de succès. Les lettres sont des repères locaux, pas des
IDs Ensemble. Aucun choix de modèle imposé par ce plan. Le lead affine D après
avis architecte ; les autres estimations PM sont conservées. Incertitudes : coût
de migration des fixtures publiques et détection des formes réfléchies refusées.

- **A — contrat** : `engine-architect`, complexité brute/retenue **58/58**, risque
  **62**. Lecture seule, contexte fresh, sans worktree. Avis demandé pendant ce
  cadrage ; avis reçu et intégré. Avant auteur, figer alias exacts et diagnostics
  publics dans le contrat, sans réouvrir silencieusement les frontières retenues.
- **V — investigation VBO distincte** : spécialiste ressources ou correctness,
  **55/55**, risque **64**, lecture seule. Optionnelle ; peut être parallèle à A.
  Aucun correctif ne rejoint cette mission sans nécessité causale et réévaluation.
- **Q — plan QA** après A : `qa-planner`, **46/46**, risque **52**. Proposition
  texte seulement, persistée et **committée par le lead avant worktree auteur**.
- **D — auteur vertical TDD** après Q et barrière : `gpu-engineer` (OpenGL),
  PM initial **68/70**, risque **71** ; réévaluation lead PM/LLM brute/retenue
  **78/78**, risque **76**, pour les 12 formes, voie entière, validation Reader et
  protection du buffer leurre désormais explicites. Un seul auteur pour Reader,
  Setter, contexte concerné et tests ; contexte fresh, worktree isolé.
- **G — generator de gaps** après intégration réelle de D : `test-engineer`,
  **54/70**, risque **55**. Propositions tests uniquement ; production interdite,
  persistance et exécution attribuées au lead. KEEP/MODIFY/REJECT/DEFER avec oracle.
- **R — revue indépendante** après intégration de D : `correctness-reviewer`,
  **57/57**, risque **65**, fresh, lecture seule ; parallèle à G.
- **T — audit tests hors board d'implémentation** après intégration de G et des
  corrections : `test-reviewer`, **53/53**, risque **60**, fresh ; scénario
  discriminant avant finding haut, puis correction par propriétaire et ré-audit.
- **F — vérification et clôture** : `rendering-verifier`, **56/56**, risque **64**,
  fresh. Le coordinateur persiste la documentation et exécute les propositions QA.

Chemin critique : **A → Q → contrat/QA committés → D → intégration → G →
intégration/corrections → T → clôture F**. R avance avec G ; après le candidat
final, lecture de T et préparation de F peuvent être parallèles. Toute correction
production invalide les conclusions affectées et exige requalification.

Un écrivain pour la chaîne couplée, un build par sortie, bail GPU exclusif pour
les exécutions GL. Les lecteurs peuvent travailler en parallèle, pas deux builds
ou runs partageant le contexte/fixtures. Les consommateurs de branches isolées
restent dans le plan jusqu'au shutdown/merge, diff contrôlé et preuves accessibles.

## Preuves et conditions de sortie

- Plan QA obligatoire (comportement sous contrat et risque PM ≥ 50) : exigences
  explicites/inférées, nominal/négatif, invariants, top 3 risques, registre
  exigences ↔ preuves, setup puis 70–80 % planifiés et 20–30 % buffer.
- Risque QA = impact × probabilité (1–5) ; pour chaque produit ≥ 10 : déclencheur,
  blast radius, détection, mitigation et gap. Ce produit ne modifie pas la grille PM.
- RED causal avant production sur toolchain/contexte fonctionnels ; absence de
  contexte ou zéro test découvert = INCONCLUSIVE, jamais RED.
- GREEN minimal puis refactoring/non-régression sur le diff intégré. Tests CPU
  des arguments/rejets et preuve GL réelle du contrat retenu ; mocks ≠ rendu.
- Oracles indispensables : vraie réflexion `vec3` → composantes/type corrects ;
  signed/unsigned → IPointer ; mismatch et formes refusées sans appel d'upload/
  configuration ; location invalide et nom non tronqué ; VBO leurre intact malgré
  liaison intercalée. Vérifier le VAO et une valeur réellement consommée par un
  shader (readback ou capture numérique contrôlée), pas seulement `GL_NO_ERROR`.
- Matrice des 12 formes : oracles CPU exhaustifs et cas natifs couvrant chaque
  forme retenue. Toute case native non exécutée reste INCONCLUSIVE, pas couverte
  par analogie avec `vec3`. Fixtures RAII de test nettoyant VAO/VBO même après ASSERT.
- Identité candidat, commandes, comptages, skips, erreurs GL et artefacts conservés.
  Suite native finale non vide, findings résolus ou explicitement arbitrés.
- ASan à définir séparément avec préflight et périmètre ; aucun héritage du PASS
  historique. Pas de performance ni d'équivalence visuelle déduite des tests CPU.

## Préconditions humaines et documentaires

1. Présenter les choix de contrat ci-dessus (12 formes, rejet à l'utilisation,
   setter autonome ciblant son VBO) et figer diagnostics/alias dans le plan QA.
   Si la migration devient une refonte large, revenir à une révision du périmètre,
   pas ajouter implicitement une seconde mission au correctif.
2. Vérifier un canal de restitution QA autorisé. Un texte local BLOCKED non livré
   au lead n'est pas un livrable ; ne pas contourner les permissions par execute.
3. Préparer une **nouvelle mission native**, présenter sa politique avec
   `/ensemble-mission config`, puis attendre `/ensemble-mission start` de
   l'utilisateur. La politique F7 v3 n'est pas automatiquement celle de la suivante.
4. Avant publication future : contexte explicite, deux scores, affinités et vrais
   IDs des producteurs intégrés. Aucun travail auteur pendant ce cadrage.

Statuts conservés : ADR-0003 **Accepted/partial**, ADR-0005 **Accepted/partial**,
ADR-0008 vérifié Windows Debug seulement ; ADR-0001/0002/0004/0006/0007 **Proposed**.
Aucun ADR-0009 créé. Voir [[adr/README|le registre]].

## Sources et limites de l'enquête

QMD limité à `artist-wiki` avec intent explicite ; notes récupérées après recherche.
GrepAI utilisé pour localiser création/destruction VBO et chemin Attribute/Uniform ;
Serena initialisé sur la racine ARTIST pour inspecter symboles et stockage.
Les aperçus de templates ne suffisent pas : des lectures ciblées ont complété
les résultats. Une trace vide n'est pas une preuve d'absence de consommateur.
Les limites d'analyse C++ sans base de compilation fiable restent applicables.

Rapport préparatoire PM : session `ses_eb1699079ffei4DLWxA1yJbmZa` ; architecte :
`ses_eb169af2fffe3mnb4B0YRJVBSd`. Deux analyses en lecture seule, sans tests ni
modification. Le coordinateur retient une seule des deux tranches architecte
pour la mission immédiate, conformément au bornage PM ; le lifetime VBO est séparé.
