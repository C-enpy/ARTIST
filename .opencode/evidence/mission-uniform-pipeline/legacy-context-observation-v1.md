# Raccordement local ratifié — observation des contextes CTest, v1

## Mandat et portée

Décision du coordinateur après proposition architecte `result_mvuc90ln_001q_zom79m`. Complète le contrat et la précision de cible matérielle sans adoption globale d'ADR. Conserver les deux sélections préparation/lifetime sous Windows et Linux : garde du contexte effectivement utilisé avant programme/upload/draw, pas préflight dans un autre contexte.

Pendant la mission : tests synthétiques/processus factices seulement. Fixture C++ et GL non compilés/non exécutés. Aucune dépendance, backend, campagne ou nouvelle variante CMake. Hors activation explicite, comportement historique inchangé.

Le plan QA de cette extension doit être persisté/commité avant son auteur. Ce document ratifie les entrées, pas une admission d'auteur immédiate.

## Transport déclaré, sans configuration cachée

Les adaptateurs transmettent par `CommandSpec.env_delta`, depuis le plan et les reçus vérifiés :

- `ARTIST_VERIFICATION_CONTEXT_V1=opengl-hardware` (activation) ;
- `ARTIST_VERIFICATION_RUN_ID` ; `ARTIST_VERIFICATION_PLAN_SHA256` ;
- `ARTIST_VERIFICATION_REQUEST_ID` (unique par invocation CTest, pas par cas) ;
- `ARTIST_VERIFICATION_EXPECTED_GL_VENDOR` ; `ARTIST_VERIFICATION_EXPECTED_GL_RENDERER`.

Aucune déduction depuis un nom de GPU, aucun fallback logiciel. Paire exacte non vide, sans contrôle, maximum 4096 scalaires ; espaces conservés. Sous Windows, lecture Unicode wide explicite pour ces entrées, sans supposer la page ANSI du processus de tests. Linux : UTF-8 strict. La cible reste dans l'empreinte du plan. Ces seules variables publiques peuvent être conservées dans les preuves ; jamais l'environnement global.

Au lancement par le pipeline, les six noms réservés hérités du parent ne valent pas déclaration : les retirer de l'environnement enfant de base puis appliquer les seules valeurs explicites de `CommandSpec.env_delta`. Ne pas modifier le parent. Ce filtrage limité empêche une activation ambiante d'un cas CPU/logiciel ; les autres variables ordinaires restent soumises au contrat existant.

Activation absente et aucun champ présent : voie historique. Activation/identités/paire partiellement présentes, invalides ou incompatibles : refus avant corps du test, pas voie historique silencieuse.

## Observation par cas

Une ligne dédiée par cas OpenGL activé dans stdout, conservée dans son `system-out` JUnit : préfixe exact `[ARTIST_CONTEXT_V1] ` suivi d'un objet JSON sur une seule ligne. JSON strict, échappement correct des chaînes, pas d'objets partagés concurrents.

Champs obligatoires : `schema_version=1`, `kind="opengl-context"`, `run_id`, `plan_sha256`, `request_id`, `ctest_name`, `profile="opengl-hardware"`, `expected_gl_vendor`, `expected_gl_renderer`, `gl_vendor`, `gl_renderer`, `gl_version`, `context_major`, `context_minor`, `context_profile_mask`, `context_current`, `qualification`, `precondition`, `reason`.

`ctest_name` est le nom réel `opengl.<suite>.<test>` du cas courant GoogleTest, non une valeur copiée depuis l'environnement. Attribution exacte attendue par le collecteur.

États : `qualification=MATCHED_EXPECTATION|UNVERIFIED`, `precondition=OK|UNAVAILABLE|INVALID_REQUEST`. `reason` est une chaîne diagnostique bornée, non un message interprété comme commande. Une observation indisponible garde des champs observés `null`/`false`, jamais des valeurs inventées. Identité malformée ou observation absente ne peut pas devenir preuve valide par simple code de sortie.

La fixture copie les valeurs du contexte créé sur son thread. Vérifier contexte courant égal à la fenêtre possédée, version numérique >=3.3, masque core non ambigu, correspondance exacte fabricant/renderer et refus des logiciels connus. Observation avant toute création de programme/upload/draw dans le cas. Pas de preuve de résidence physique universelle ni de capacité avancée déduite des chaînes.

## Refus avant travail et suites dérivées

Si la demande est invalide ou le contexte indisponible/discordant, produire le diagnostic disponible et empêcher le corps du test. La stratégie GoogleTest doit propager ce refus à travers les `SetUp` dérivés : un retour/skip dans la base seul ne garantit pas l'arrêt du code du `SetUp` appelant. Le chemin `PreparedFloatUniformComponentTest::SetUp` doit être contrôlé explicitement, avant sa boucle glGetError en absence de contexte.

Les scénarios négatifs détachant volontairement le contexte après qualification restent inchangés. Aucun garde global ne doit les neutraliser. Destruction et TearDown conservent leur ownership ; l'observation ne transporte aucun pointeur ni emprunt GL.

## Collecteur

Exiger une et une seule observation attribuée pour chaque cas `opengl.*` sélectionné sous hardware, jamais pour les cas `unit.*`. Réconcilier découverte/sélection/JUnit et préserver tous les codes/logs bruts. Plusieurs observations, JSON malformé, identité étrangère ou données contradictoires : panne protocole (4), pas succès ou précondition devinée.

Une précondition indisponible structurée et un arrêt natif préalable cohérent donnent BLOCKED/INCONCLUSIVE (3). Un skip ordinaire requis reste non favorable et ne devient pas ce cas sans observation conforme. Une vraie assertion échouée reste FAIL (5) ; si d'autres cas sont indisponibles, conserver les deux causes et la population partielle, sans transformer l'échec en inconclusif. Respecter la priorité d'agrégation ratifiée.

Une observation conforme n'annule ni failure/skip du cas ni code enfant incompatible. Ce format n'est pas le JSON complet des workloads : aucun compteur d'opérations, échantillon ou mesure ajouté.

Avant intégration du producteur/collecteur équivalent, les sélections hardware non qualifiables doivent être refusées avant tout enfant CTest. Ne pas présenter un BLOCKED après exécution comme garde préalable.

## Ownership et validation

Après QA : auteur GPU désigné uniquement pour `ARTIST/component-tests/include/OpenGLComponentTests.hpp`, support local nouveau sous ce dossier si utile, et l'arrêt préalable du seul `SetUp` dérivé concerné dans `PreparedFloatUniformComponentTests.cpp`. Corps des tests, assertions et API moteur hors édition. Pas de CMake nécessaire. Toute autre édition historique requiert un nouveau transfert explicite.

Parser d'observation Python pur et ses tests : nouveau module `scripts/verification/context_evidence.py`, tests dédiés ; ownership à attribuer avec le producteur avant écriture. Réemploi des utilitaires intégrés permis, pas deuxième parseur workload.

Interface fixée avant auteur : `parse_context_observation(text: str, *, expected: dict) -> dict`. `text` contient le `system-out` d'un seul cas. Le parser exige exactement une ligne au préfixe ratifié et retourne son objet JSON validé ; données absentes, ambiguës, malformées ou étrangères : `ValueError`. `expected` contient exactement `run_id`, `plan_sha256`, `request_id`, `ctest_name`, `profile`, `expected_gl_vendor`, `expected_gl_renderer`. Les noms `schema_version` et `kind` sont contrôlés par le format, pas un second schéma workload. Construction/import/parsing sans I/O, processus, réseau ou chargement GL. L'interprétation des statuts de précondition et des assertions JUnit reste au collecteur.

Auteur core : raccordement `_junit`/états après intégration du parser/producteur, injection explicite pour tests isolés, pas copie de branche. Adaptateurs : transport des six variables et déclarations d'effets après intégration des interfaces communes. Le lead possède les barrières et transferts.

Contrôles attendus : observation nominale, absente, dupliquée, malformée, étrangère ; chaînes exactes/Unicode et limites ; masque/version/currentness ; logiciel même si attendu ; cas CPU sans observation ; précondition vs assertion vs skip ordinaire ; sorties/codes bruts ; `list/plan` purs et opt-in absent inchangé. Les tests synthétiques ne prouvent pas la garde GL native : revue indépendante du code et NON_EXECUTE explicite requis.
