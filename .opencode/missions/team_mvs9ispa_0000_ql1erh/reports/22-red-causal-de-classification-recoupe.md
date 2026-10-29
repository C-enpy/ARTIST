# QA Linux — RED causal de classification recoupé

## Résultat attendu avant correction obtenu

Le nouveau test moteur trouve bien les attributs sous leurs noms natifs indépendamment observés, puis échoue sur la classification des tableaux. Le lead a relu et archivé les résultats existants, sans relancer le test.

**Un test sélectionné et exécuté, un cas en échec, zéro error/skip, CTest8.** Compilation C++ et compilation/link des shaders réussissent ; le contexte est Mesa25.2.8/llvmpipe, OpenGL4.5core.

Le journal distingue les cas :
- Scalaire `QA_LONG_ATTRIBUTE`, size1 : isGLArrayfalse, attendu false, témoin réussi.
- Array1 `qaArray`, size1 : la clé native est bien publiée, mais isGLArrayfalse au lieu de true.
- Array2 `qaArray2`, size2 : même défaut du bit, indépendamment du refus déjà possible par la taille.

Les assertions en échec portent donc sur **isGLArray**, pas sur une clé codée en dur `[0]`, un shader illégal, une toolchain absente ou un contexte indisponible. Ce RED est distinct des trois échecs historiques d'orthographe conservés dans leurs archives.

## Attribution et phase suivante

L'auteur a exécuté le test avant modification du Reader et annonce également un RED CPU sur les nouveaux mocks/diagnostics après compilation ; les détails causaux de cette seconde suite restent à recouper. Le lead n'attribue pas à sa propre exécution ces résultats : il a seulement analysé les XML/logs et copié les preuves.

L'auteur poursuit désormais le GREEN minimal selon son contrat, sans GO intermédiaire : gates effectifs, classification interne moderne, clés publiques intactes et staging conservé. Aucun GREEN, résultat de non-régression ou PASS de parité n'est annoncé à ce stade.

Les garanties restent bornées : refus avant mutation nativeGL, pas de refonte du wrapperCPU ou de nouvelle provenance de métadonnées. Les tests de gates négatifs, réponses malformées et erreur tardive restent nécessaires ; le RED natif ne les prouve pas tous.

## Préconditions d'auteur confirmées

L'auteur a confirmé la lecture du contrat et du complément QA avant toute édition. Le problème initial de cwd canonique et le refus externe unique ont été résolus par session_move natif après la transition autorisée ; la politique Context Mode locale a été effectivement lue. Aucun refus répété, permission ajoutée ou MCP reconfiguré. Le lead a clarifié l'ancienne mention DEFERRED, désormais remplacée par la publication effective et les baux privés reçus.

## Annexe technique

- Test : `opengl.AttributeQaPreflightTest.NativeReaderArrayOneRegression`, cible composant réelle dans `/home/artist/src/ARTIST-attribute/out/build/linux-debug/`.
- Artefacts auteur relus : `/home/artist/src/ARTIST-attribute/out/logs/attribute/red.xml`, `red.log`, `red-build.log`.
- Copies lead sans écrasement : `out/logs/mission-linux-qa/attribute-correction-red/`.
- SHA256 XML `d296d95f9ff0b020a72452359bfaf78a996ccac5eae47367609ff69dd8dc3d88` ; log `890b0cbf3cc31cc3be9e57633669db2eca39741a677587731d597151dbcb9bc4` ; build `347114f80902bf6c90e034a87aa355817f97989e3e29fdecef8d29e53b19a506`.
- Méthode lead : PythonXML/JSON et lectures ciblées via Context Mode, aucun build ou contexteGPU supplémentaire. Le log de build se termine par le lien réussi du binaire composant.
- Auteur : pool-9/gpu-engineer, tâche `task_mvsggywg_0028_23u2s9`, baux privés en cours. Le correctif et ses identités finales ne sont pas encore intégrés.
