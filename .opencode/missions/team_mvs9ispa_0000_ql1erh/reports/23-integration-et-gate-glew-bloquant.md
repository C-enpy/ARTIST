# QA Linux — correction intégrée, défaut de gate détecté en revue

## État du candidat

La correction a été intégrée à `0a4a671`, après relecture du diff, recoupement des quatre sources exécutées et des26hashes du manifeste. Les quatre archives auteur contiennent588tests uniques par configuration,512CPU+76GL, aucun échec/error/skip ; six testsCPU et un testGL ajoutés, aucun retiré. La correspondance de noms Windows/Linux est bijective après la seule différence de castGLM déjà identifiée.

Cela ne suffisait pas à conclure : la revue indépendante a trouvé un défaut du contrôle de capacité. **Le candidat n'est pas accepté globalement et les nouvelles admissions sont suspendues.** Les deux reviewers déjà en lecture seule ne sont pas interrompus.

## Défaut de capacité confirmé dans la dépendance

Le booléen `GLEW_ARB_program_interface_query` peut être vrai après chargement réussi des fonctions lorsque glewExperimental est actif, même si le contexte n'annonce pas cette extension. Notre fixture active ce mode. Le contrat exige pourtant une capacité effectivement supportée, et pas seulement des adresses de fonctions présentes.

Le lead a relu le source GLEW2.2 réellement construit : l'initialisation expérimentale écrase ce booléen après six résolutions ; l'information d'annonce est conservée séparément et exposée par glewGetExtension. Le flag de version4.3 est, lui, calculé depuis la version du contexte et n'a pas d'affectation expérimentale ultérieure dans cette version vérifiée.

Le test discriminant manquant doit dissocier : core4.3absent, annonceARBabsente, flagchargeurvrai et pointeurs présents. Attendu : succès legacy, zéro requête moderne et clé exacte conservée. Le mock actuel ne distingue pas ces deux booléens.

## Échec d'intégration Windows et expérience discriminante

Le premier passage Windows canonique a réellement échoué avec des violations d'accès dans les consommateurs CPU du mock partagé. Le build n'avait recompilé que deuxCPP malgré les headers modifiés. Résultat et JUnit ont été archivés, aucune ancienne sortie n'est présentée comme réussite.

Hypothèse testée sans changer une source : objets compilés incohérents après modification d'un layout/mock et d'implémentations inline. Le lead a nettoyé les sorties de projet par la cibleclean existante, puis relancé le même wrapper avec un job. **63CPP reconstruits,588/588réussis.** Cette expérience distingue l'échec du binaire incrémental de celui d'une reconstruction cohérente ; elle n'explique pas exhaustivement pourquoi les dépendances n'avaient pas déclenché toutes les compilations. Les futurs passages de cette réparation seront reconstruits proprement, sans changer assertion, timeout ou CMake pour masquer l'écart.

Ce GREEN sur reconstruction complète ne corrige pas le gateGLEW : ce second défaut reste bloquant pour le contrat et doit être corrigé enTDD.

## Réparation locale bornée décidée

Le lead reprend la responsabilité d'une petite réparation indivisible pendant la suspension des nouvelles admissions : test négatif et interposition du mock d'abord, REDcausal sur ce seul gate, puis substitution de l'information d'extension annoncée dans le Reader. Le contrat et le planQA existants A2 couvrent déjà exactement ce risque ; aucune nouvelle API, architecture ou garantie n'est adoptée.

Ownership : Reader, mockGL et testsReader, aucun auteur actif sur ces fichiers. Aucun changement Setter/wrapper, CMake, niveauminimumOpenGL ou instrumentation. Évaluation locale PM/LLM : complexité brute62/retenue70, risque72, incertitude liée à l'interposition et au cacheGLEW. Modèle de session et politique approuvée inchangés ; aucun nouvel agent ou override.

AprèsGREEN, le finding sera renvoyé au reviewer pour ré-audit. La recherche de gaps déjà active peut continuer. La CI du candidat0a4a671 n'est pas une preuve du futur correctif ; une nouvelle source justifiera un contrôle distant ultérieur, pas un rerun aveugle.

## Limites et preuves

- Revue : pool-10/correctness-reviewer, tâche `task_mvsidz83_0000_roea85`, finding bloquantP2 établi par analyse de code, pas reproduit sur GPU. Aucun edit/test autonome du reviewer.
- Sources primaires GLEW : `out/conan2/p/b/glew15bda264c9bea/b/src/src/glew.c:17954–17959,18052–18062,18369,12220–12231` ; version4.3 calculée à18005. Fixture expérimentale : `ARTIST/component-tests/include/OpenGLComponentTests.hpp:38`.
- Premier échec lead : `out/logs/mission-linux-qa/final-integrated-windows-normal.log` et `final-integrated-windows-normal-fail.xml`.
- Reconstruction sans changement : `integrated-clean-baseline-clean.log`, `integrated-clean-baseline-windows.log` et `.xml` dans le même dossier, code0 et588cas.
- Les archives auteur sont sous `attribute-author/`, identité complète du premier deltaRED historique manquante explicitement conservée. Ne pas lui attribuer un hash final rétroactivement.
- Consultation de rafraîchissement des profils refusée une fois, non retentée. Les profils et règles natifs précédemment observés ont été réutilisés avec les champs de routage explicites ; le generator test-engineer est effectivement routé, lecture locale et message reçus. Cela ne prouve pas un rechargement de permissions ou une sandbox. Aucune source de configuration modifiée.
