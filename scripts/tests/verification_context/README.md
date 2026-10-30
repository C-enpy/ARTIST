# Observation de contexte historique — SOURCE_READY

## Contrat et périmètre

Entrée de branche : `50af7923ff9094ac8e7a175d3256126204b4da41`.
Contrats locaux inchangés :

- `.opencode/evidence/mission-uniform-pipeline/legacy-context-observation-v1.md` ;
- `qa-plan.md`, section 10, dans le même dossier ;
- `hardware-expectation-v1.md`, dans le même dossier ;
- `.opencode/missions/team_mvu0gbxm_0000_cwsmsw/author-briefs.md`.

Le module expose `parse_context_observation(text, *, expected) -> dict`.
Après le RED causal du socle nominal, le candidat GREEN valide le JSON unique,
les champs v1, l'attribution, les types/chaînes et la cohérence de qualification.
Les chaînes et diagnostics sont bornés à 4096 scalaires Unicode. Les cibles
restent exactes, sans strip ni normalisation ; le diagnostic peut contenir des
contrôles échappés en JSON, sans interprétation. Aucun raccordement core,
adaptateur ou scénario. Le producteur C++ décrit ci-dessous reste SOURCE_ONLY.

Plan borné : tests discriminants et socle nominal → RED causal observé par le
lead (reçu) → GREEN minimal autorisé (candidat présent) → GREEN/non-régression
par le lead (reçu) → producteur natif SOURCE_ONLY autorisé (candidat présent),
puis revue indépendante avant réception.
L'auteur n'exécute aucun test. Aucun CMake/Conan/CTest, compilation native,
contexte GL, moteur, rendu, benchmark, installation ou cloud.

## Oracles et couverture attribuée

28 méthodes `test_*` sont présentes statiquement ; sous-cas supplémentaires
par mutations unitaires. Ce nombre n'est pas une découverte/exécution.

- A3 : préfixe et cardinalité, JSON strict (doublons/constantes/types), champs
  obligatoires, version de schéma, identités et attribution exacte. Un OK
  exige contexte courant 3.3+ core non ambigu, cible exacte non logicielle.
  Les observations UNVERIFIED/UNAVAILABLE et INVALID_REQUEST structurées
  restent des données, pas une exception de test ni un succès matériel.
- A4 : cible exacte, espaces et Unicode sans normalisation ; 4096 scalaires
  acceptés, 4097/contrôles/NUL/surrogates/types invalides refusés. Guillemets
  et backslashes restent des données. Le transport des six variables relève
  du producteur/adaptateur, pas de ces tests du parser.
- A5 : identité d'un seul cas `opengl.*`, profil matériel et correspondance
  exacte ; aucune conversion de `unit.*` en preuve GPU. La population globale
  découverte/sélection/JUnit et l'absence d'exigence CPU sont au collecteur.
- A8 : reparsing sans cache, cas distincts, mutation de la sortie et de
  l'attendu ; sentinelles d'I/O/processus/réseau/environnement/import natif
  pendant import applicatif et parsing. La lecture de source nécessaire au
  chargeur Python n'est pas une opération d'I/O applicative du parser.

Les codes d'agrégation et les assertions/skip JUnit (A6), `list/plan` (A7),
activation et garde des fixtures natives (A1/A2) ne sont pas prouvés ici.
Pas de second schéma workload, de mesure, ni de déduction matérielle universelle.

## Commande réservée au runner lead

Depuis la racine de ce worktree, via Context Mode :

```powershell
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_context -p 'test_*.py' -v
```

RED rapporté par le lead sur `886354a6f6320033a9ea6dc85bdb2ea88c7bf30e` :
28 tests exécutés, 167 failures assertions/sous-cas, zéro error/skip, code 1 ;
import et cas nominal fonctionnels ; notamment
`test_a3_duplicate_observation_is_ambiguous_even_if_identical`,
`test_a3_each_attribution_field_must_match_expected` et
`test_a3_known_software_cannot_match_even_if_exact_target` échouent par
`AssertionError: ValueError not raised` contre le socle non validant.
La méthode de pureté échouait sur doublon accepté, pas sur effet détecté.
Preuve indiquée par le lead :
`out/verification-synthetic/legacy-context-red-2026-10-30T02-37-50-773Z/`.
Cette preuve n'a pas été exécutée par l'auteur ni recopiée dans ce worktree.

GREEN rapporté par le lead sur `f2da69b91853661dbf8b9409237a850585f8fda6` :
28/28, zéro failure/error/skip, code 0, empreintes stables et tests inchangés
depuis le RED. Preuve indiquée :
`out/verification-synthetic/legacy-context-green-2026-10-30T02-41-29-261Z/`.
Cette exécution ne prouve ni la fixture native ni le transport des variables.

## Producteur C++ SOURCE_ONLY — ordre et oracles de lecture

Fichiers natifs possédés :

- `ARTIST/component-tests/include/OpenGLComponentTests.hpp` ;
- support local nouveau `OpenGLContextObservation.hpp` dans ce même dossier ;
- uniquement la garde avant `glGetError` du `SetUp` dans
  `ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformComponentTests.cpp`.

Ordre du candidat :

1. Lire uniquement les six noms déclarés, copier leurs valeurs localement et
   distinguer absence/chaîne vide/entrée illisible. Toutes absentes : retour de
   lecture avant sonde GTest/GL, puis chemin historique de création/log/destruction.
   Windows utilise `GetEnvironmentVariableW` et conversion UTF-16 stricte ; Linux
   copie les octets puis exige UTF-8 strict. Bornes scalaires, contrôles et cible
   exacte réutilisent les helpers intégrés de `WorkloadProtocol.hpp` en lecture,
   sans appeler son parser/runner/writer ni recopier son schéma workload.
2. Valider activation complète, identités et paire avant `glfwInit`. Le nom du
   cas provient exclusivement de `current_test_info()` : `opengl.<suite>.<test>`.
   Si l'attribution manque/est invalide : diagnostic brut explicite
   `[ARTIST_CONTEXT_INVALID_REQUEST_V1]`, pas d'identité inventée ni de ligne
   qualifiable. Le collecteur doit refuser cette observation absente. Si seule
   l'activation est incompatible mais l'attribution valide : INVALID_REQUEST
   structuré, champs observés null/false, sans initialisation GLFW.
3. Initialiser GLFW, créer la fenêtre, rendre son contexte courant et vérifier
   son égalité à la fenêtre possédée avant GLEW. Les échecs opt-in produisent
   UNVERIFIED/UNAVAILABLE ; hors opt-in, les exceptions historiques restent.
4. Après GLEW : revérifier le contexte courant, copier vendor/renderer/version,
   refuser chaînes inconnues/invalides, borner la purge des erreurs d'initialisation
   à 16 lectures. La version observée par GLFW gate les queries numériques 3.0 ;
   la version numérique obtenue gate le masque 3.2. Une erreur de query ne laisse
   pas une valeur par défaut présentée comme mesure. Exiger >=3.3 et masque core
   exact, puis cible exacte et absence de renderer logiciel connu.
5. Émettre une seule ligne JSON préfixée, construite en données locales avec
   échappement strict, avant programme/upload/draw du cas. Les chaînes/nombres
   indisponibles restent null, le booléen current reflète le contrôle effectué.
   Aucune capacité avancée ni accélération physique universelle n'est déduite.
6. Refus : émission → enregistrement `GTEST_SKIP` → exception publique
   `testing::AssertionException` pour dérouler tout `SetUp` dérivé. Le source
   GoogleTest 1.14.0 disponible et le pin CMake ont été relus : avec capture
   d'exceptions activée, ce type est intercepté sans failure supplémentaire,
   le corps skipped est ignoré et TearDown est appelé. Ce mécanisme **n'a pas
   été compilé/exécuté ici**. Capture désactivée : pas de retour vers le corps,
    mais arrêt exceptionnel possible ; TearDown et JUnit complet ne sont alors
    pas garantis. Ce n'est pas un skip propre ni une preuve favorable.
7. Le `SetUp` PreparedFloat possède aussi `if (IsSkipped() || HasFatalFailure())
   return;` avant sa boucle GL. Les corps/assertions, y compris le détachement
   volontaire ultérieur de contexte, sont inchangés. `window` reste possédé par
   la fixture ; le support ne possède aucune ressource GL et ne stocke aucun
   pointeur de contexte dans l'observation. TearDown reste textuellement inchangé.

Oracles pour revue indépendante : absence de GL avant garde current ; refus
avant programmes ; ligne unique et vrais noms GTest ; branches historiques
intactes ; skip et propagation distincts d'une assertion échouée ; nettoyage
des créations partielles par la même fixture. Les trois cas catalogués restent
la portée de qualification prévue : les autres fixtures et leurs TearDown ne
sont pas réécrits ni qualifiés par ce travail.

État auteur : Python **NON_EXECUTE** ; RED et GREEN du parser acquis selon
restitutions du lead. Non-régression finale réservée au runner lead.
C++ **NON_COMPILE/NON_EXECUTE** ;
GL/image/performance **NON_EXECUTE**, aucun gain revendiqué. Les preuves de
runner et la revue indépendante restent à produire avant réception complète.

## Réception du coordinateur — portée intégrée

Le candidat source `057b8417933cdcbe10b9c16e3dffd4c56acd569a` a reçu une revue
indépendante SOURCE_ONLY sans finding bloquant établi, puis a été intégré dans
`91c5ac23bf17d4d88dd36699cb7d15a6d6835872`. Les six fichiers reçus ont été comparés
au candidat revu sans divergence. Le coordinateur a exécuté les 28 tests Python
dans l'arbre intégré : 28 satisfaits, aucun échec, erreur ou skip, empreintes
stables. Preuves : `out/verification-synthetic/legacy-context-integrated-2026-10-30T03-07-08-478Z/`.

La revue n'est pas exhaustive et ne prouve pas une exécution native. Le C++,
Windows/Linux et OpenGL restent NON_COMPILE/NON_EXECUTE. Collecteur, adaptateurs
et qualification du pipeline complet ne sont pas couverts par cette réception.
