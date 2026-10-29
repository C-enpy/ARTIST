# Plan QA actif — float préparé, réconcilié avant auteurs

Date : 2026-10-27. Statut : réconcilié et validé par le lead, à porter dans le
commit H0 avant auteurs. Aucun comportement du futur accès n'est encore testé.
Auteur de la proposition actuelle : qa-planner natif OpenAI, session
ses_e82cd2b71ffeDxUbgW6PUuLh1C, réponse finale publique
msg_17d3440ed0012NHVdJIJ0Svs3y. Persistance/réconciliation/runner : lead.
Lire contract.md pour l'adoption utilisateur et task-preview.md pour les barrières.

## Admission et limites

Qualification de cette nouvelle tâche contrôlée par le lead : tâche structurée
qa-planner/fresh/worktree:false/final_response, modèle natif openai/gpt-6.1-sol
high, permissions de session [] sans overrides hérités. Le profil natif résolu
ARTIST a ses règles restrictives ; contexte initial avec une seule entrée user,
pas d'historique repris. Lectures locales completed réellement observées, dont
AGENTS.md, contrat, plan, profil QA et sources de tests ; skill chargé.

La session a terminé succeeded, réponse finale finish=stop avec completed présent.
Ensemble a pourtant annoncé ne pas récupérer le candidat final. Le lead a lu et
contrôlé le texte public complet via l'API native autorisée de cette même session,
sans appel QA à un transport interdit, nouvel agent, grant, handoff ou réparation
du harness. Cette restitution est reçue et validée par le lead ; l'absence de
candidat retenu par Ensemble reste une limite d'orchestration distincte, non un
PASS de cette fonction. La complétion de tâche devra passer par la voie native
du lead après H0 ; si cette voie refuse, ne pas contourner le refus ni publier
l'auteur. Aucun certificat de sandbox ou test de tous les droits interdits.

L'ancienne proposition QA ses_e838b2cb0ffeKTjoF0uoSPLjT8 et le test Go429 restent
historiques/conditionnels, pas les preuves de la qualification actuelle. Garder
un seul plan actif ici. Les rapports02/03 conservent leurs constats datés.

## Exigences

Explicites : préparer après link/reflection, accès float typé, absent/type/stale
traités, rejet avant danger, aucun lookup répété, compatibilité par nom,
consommateur existant migré, readbacks et résultat dessiné équivalents, RED avant
code, régressions et revue indépendante, aucune performance sans mesure.

Précisées par l'adoption : scalaire strict (array[1] rejeté), miroir CPU conservé,
témoins locaux sans ownership GPU, moves suivant les contexts uniques, target
migré et decoy legacy dans le component-test, pixels du programme f2Scalar.

Explicitement adoptées aussi : publication atomique, préservation après rejet
avant effets, ordre des gardes, récupération après mauvais programme et contrat
mono-thread non réentrant. Aucun rollback GL global n'est promis.
Inférences/recommandations QA : vérifier la sensibilité des compteurs, refuser
l'égalité de deux images vides, compléter ASan par revue et preuve GL réelle.

## Risques I × P (échelles 1–5, pas scores PM)

- Lifetime/stale : 5×4=20. Déclencheurs : destruction, maps, free/reload, moves,
  réutilisation d'ID. Blast radius : UAF ou écriture vers ancien programme.
  Détection : ordre des gardes, appels interdits, ASan et GL réel. Mitigation :
  témoins/générations vérifiés avant pointee. Gap : aucun test nouveau exécuté.
- Mauvais programme/contexte : 5×3=15. Déclencheur : leurre même location, zéro,
  contexte absent/différent. Blast radius : écriture dans ressource tierce.
  Détection : valeurs target/decoy et état courant conservés ; gardes avant driver.
  Mitigation : vérifications par appel conservées. Gap : nouveau chemin non testé.
- Mauvaise préparation/résolution répétée : 3×4=12. Déclencheur : absent/optimisé,
  mauvais type/cardinalité, array[1], ancienne réflexion, updates répétés.
  Blast radius : valeur erronée ou déplacement de travail non réalisé.
  Détection : négatifs et observation séparée des résolutions CPU/native.
  Mitigation : type exact, publication estampillée, descripteur préparé immuable.
  Gap : exécution du protocole et de l'oracle fixés ci-dessous encore à produire.

## Registre obligations ↔ preuves proposées

1. Préparation/type : tests CPU/compilation et GL réel sur float actif, absent,
   uniform déclaré mais optimisé, GL_BOOL/int/vector/matrix/double, array[1] et
   cardinalité >1. Vérifier diagnostics, aucune publication/upload indésirable.
2. Résolution amortie : observer préparation puis N updates distincts, sans
   nouvelle recherche de map CPU, glGetUniformLocation ou any_cast. Un compteur
   natif seul ne couvre pas le lookup CPU. Contrôle négatif sensible à la
   réintroduction d'une résolution ; instrumentation bornée de test, pas framework.
3. Ownership : accès survivant à owner détruit sans prolonger le programme ;
   contrat de copie/move et absence de pointee mort inspectés, ASan complémentaire.
4. Invalidation : add/replace map vide et non vide, autre nom, même nom conservé,
   candidat rejeté, métadonnées, free/reload, même ID simulé, destruction et moves
   vers destination chargée. Rejet répété sans effet ; nouvel accès valide après
   nouvelle préparation. Échec même programme vivant distinct d'échec après retrait.
5. Contexte/programme : leurre même location, programme zéro, contexte absent ou
   incompatible. Aucun upload, miroir CPU ou état courant modifié par un rejet.
6. Compatibilité : API par nom et diagnostics antérieurs conservés ; valeurs CPU
   et GL cohérentes après alternance de mises à jour legacy/préparées.
7. Consommateur/pixels : vrai programme existant, séquence et états identiques,
   pixels non vides attendus, égalité ancien/nouveau, modification de float visible.
   Appliquer géométrie, framebuffer, états et samples fixés ci-dessous, sans
   référence ajustée après résultat ni nouvelle infrastructure générale d'image.
8. Livraison : admission attestée, H0 avant worktree, RED causal puis GREEN,
   preuves du candidat intégré et documentation bornée. Candidat différent,
   précondition absente ou zéro test ne produisent pas PASS.

## Oracle pixel fixé avant auteur

Programme existant f2-single-values.vert et fragment minimal.frag ; triangle
(-.75,-.75,0),(.75,-.75,0),(0,.75,0), attribut0=trois floats, VAO/VBO explicites.
FBO32×32, GL_RGBA8 linéaire mono-échantillon complet, draw/read buffer
GL_COLOR_ATTACHMENT0. f2Matrix identité, f2Vector et deux éléments f2Array à zéro,
testUniform=1. L'initialisation native de l'array appartient au setup et ne justifie
pas d'élargir le setter single-value. f2Scalar vaut0 puis.5, gl_Position.w=1.

Viewport0,0,32,32 ; fill, color mask complet. Désactiver blend/dither/sRGB/
multisample/cull/scissor/depth/stencil/rasterizer-discard/polygon-smooth/
color-logic-op/sample-coverage/alpha-to-coverage. Clear(0,0,0,1) avant CHAQUE rendu.
GL_PIXEL_PACK_BUFFER=0, pack alignment1, row-length/skips nuls ; lecture RGBA/
UNSIGNED_BYTE du FBO explicite. Erreurs GL contrôlées après setup/upload/draw/read,
jamais purgées dans l'intervalle mesuré.

Origine de glReadPixels bas-gauche et centres(x+.5,y+.5). Sommets écran initiaux
(4,4),(28,4),(16,28), puis(12,12),(36,12),(24,36) avant clipping, z=.5 valide.
Attendus fixes, sans tolérance : blanc(255,255,255,255), fond(0,0,0,255).
- (8,8) : blanc à0, fond à.5 ;
- (24,24) : fond à0, blanc à.5 ;
- (18,16) : blanc dans les deux cas, intérieur commun non vide ;
- (2,2) : fond dans les deux cas.

Chaque route legacy/préparée satisfait d'abord ces attendus indépendants puis leur
égalité. Supprimer le second update doit casser les pixels qui changent ; tout
rendre noir doit casser l'intérieur commun. Attendus calculés statiquement avant
code, pas calibrés sur une image ; aucun résultat GPU nouveau à ce stade.

## Observation des résolutions fixée avant auteur

Distinguer C=résolutions/recherches CPU par nom (contains/at/find réels, pas
seulement un appel logique), L=glGetUniformLocation et A=sites d'extraction
type-erased de UniformContext::getValue/any_cast et autres sites atteignables.
Compteurs passifs bornés au mode mock existant aux sites CPU pertinents, attentes
du mock GL existant ; pas de nouveau framework/cible, modification de STL ou API
produit d'instrumentation. Le code observé reste celui de l'upload testé.
Revue des corps/graphe d'appels pour exclure contournement via getUniforms,
extraction alternative ou différence de branche mock/réel.

Dans le test GL réel, si l'observation native est nécessaire : espion local RAII
pass-through du point d'entrée GLEW glGetUniformLocation, sauvegardé/restauré sous
bail GPU exclusif, sans remplacer le driver. Ce dispositif reste dans le test.

Intervalles : setup/réflexion, préparation, puis trois updates distincts
0,.5,-.25, journalisés séparément. Seul l'intervalle updates exige ΔC=ΔL=ΔA=0,
trois uploads exacts et aucune nouvelle réflexion/résolution de type. Lire le
miroir CPU et effectuer les readbacks APRÈS cet intervalle pour ne pas contaminer
les compteurs. Une location déjà réfléchie peut ne nécessiter aucun L en préparation.

Calibrer les trois témoins séparément : lookup connu augmente C, résolution
native connue augmente L, GET legacy valide augmente A. Contrôles causaux bornés
du runner : réintroduire séparément lookupCPU, résolution native et GET effacé
dans un candidat d'expérience doit casser l'assertion respective ; archiver les
deltas à part, revenir au candidat final et vérifier son identité. Pas de mutations
concurrentes de l'état auteur ou de correctif production opportuniste par la QA.
Aucun compteur d'allocation ou benchmark n'est requis ; pas de zéro allocation promis.

Preuves existantes et rattachement inverse :
- UniformCardinalityComponentTests::ScalarVectorMatrixSingleValuesReadBack couvre
  présence statique d'un oracle legacy float, pas préparation/stale ni pixels.
- ReflectedProgramGuardCrossTests::ReflectedOwnerRejectsOtherCurrentProgramAndRecovers
  fournit target/decoy et récupération legacy ; pas lifetime du nouvel accès.
- UniformValue/Contract et UniformReader/Ownership/Cardinality/Rejection servent
  à la non-régression ; ne pas compter vector/matrix/fp64 comme nouvelle preuve float.
- 555 normaux/555 ASan a96104c : historique attribué au runner antérieur, pas
  exécution de cette mission. Les exports et revues statiques ne sont pas du GPU.
- Toute nouvelle preuve doit se rattacher aux obligations 1–8 ; signaler preuve
  orpheline, duplication et gap. Readback seul ne satisfait pas les pixels.

## Runner, commandes et budget

Commandes candidates dans model-policy.json/verification ; jamais exécution
automatique de ces chaînes. Filtres ciblés après découverte réelle. Nouveaux cpp
collectés par glob CMake : reconfiguration pour leur découverte. Un build par
sortie, bail GPU inter-processus ; RESOURCE_LOCK CTest seul n'est pas un bail global.
ASan avec wrapper/environnement normalisés, candidat et archives distincts.

Budget indicatif en unités, pas durée : 20 setup, 60 vérifications planifiées,
20 buffer ; hors setup =75/25. Planifié : lifetime20, programme/contexte12,
type/résolution/compatibilité12, consommateur/pixels10, gaps/audit6.
Pas de budget financier ni de durée garantis par les coûts du catalogue.

Auteurs : RED→GREEN minimal→REFACTOR/non-régression. Après intégration : generator
de gaps en propositions, lead persiste/relit/exécute, audit fresh hors board puis
retour propriétaire et ré-audit si nécessaire. Aucun runner QA autonome.

## Sortie

Qualification actuelle et restitution documentées ci-dessus ; commit H0 et
complétion native du lead avant worktree auteur. Oracle pixel/observabilité fixés,
obligations 1–8 à prouver sur le candidat intégré, pas déjà COVERED.
Archiver HEAD/diff/empreintes, commandes, environnement, runner, attendu/observé,
découverts/sélectionnés/exécutés/failures/skips et JUnit. Zéro test ou précondition
absente = INCONCLUSIVE, bug observé = FAIL ; aucun PASS global déduit d'un partiel.
Pas de renoncement implicite aux exigences hautes ; tout gap bloquant traité ou
arbitré explicitement. Linux/Release/autres API et performance hors preuve livrée.
