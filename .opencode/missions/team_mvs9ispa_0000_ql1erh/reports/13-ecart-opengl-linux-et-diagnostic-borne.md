# QA Linux — trois échecs OpenGL conservés, diagnostic du contrat en cours

## Objectif et problème concret

La mission vise les mêmes obligations QA sous Windows et Linux. Le premier passage graphique Linux exécute bien les tests, mais trois cas d'attributs de shader échouent : le pilote renvoie `qaArray` pour un tableau mono-élément, là où les tests attendent `qaArray[0]`. La parité Linux n'est donc pas acquise.

Décision dans le mandat autonome : conserver les échecs et faire diagnostiquer le contrat avant tout correctif. Aucun changement d'assertion, alias, skip, shader ou représentation n'est autorisé à ce stade. La campagne ASan et les helpers CI indépendants peuvent avancer sans présenter de PASS global.

## Résultats et preuves recoupés

- Candidat lead toujours `a4c0f82e1feecf5fc8dda5ab3fa3d32355c19530`, aucun diff dans ARTIST/scripts/cmake/presets ; changements préexistants du harness et des documents préservés hors ownership.
- Linux normal : CPU 506/506 annoncé par l'auteur ; le lead a relu le JUnit graphique existant : **75 noms uniques, 3 failures, 0 error, 0 skip**, soit 72/75 réussis. Pas de nouvelle exécution pour ce recoupement.
- Cas concernés : `AttributeQaPreflightTest.NativeCapabilitiesAndArraySpelling`, `AttributeQaPreflightTest.NativeReaderArrayOneRegression`, `AttributeQaRenderTest.NativeArraysDescribeThenRefuseWithoutTouchingDecoy`.
- Observation auteur : Mesa/llvmpipe LLVM20.1.2, OpenGL4.5 core Mesa25.2.8, contexte demandé3.3core ; réflexion `qaArray`, size1, GL_FLOAT_VEC3, location0. Cette observation n'établit pas encore à elle seule la conformité du pilote ou la correction du moteur.
- Windows reste vérifié au candidat indiqué : 581/581 normal et 581/581 ASan. Voir le rapport12 ; cette preuve n'annule pas le FAIL Linux.

Le manifeste initial du run porte encore INCONCLUSIVE et ne compte que les CPU après l'arrêt CTest graphique. Le lead a signalé cette lacune au propriétaire des helpers : **FAIL doit être prioritaire lorsqu'un écart est reproduit**, et les comptes du JUnit graphique doivent rester disponibles après un retour non nul. L'archive initiale reste conservée ; une correction du helper ne réécrit pas le résultat historique.

## Diagnostic spécialisé et limites

Le spécialiste OpenGL travaille en lecture seule, sans build, test GPU ou délégation ; le propriétaire de la campagne conserve seul le bail GPU logiciel Linux. Son catalogue n'expose pas QMD en phase planning. Le lead lui a donc transmis les extraits effectivement récupérés dans artist-wiki, sans réparer les MCP ni élargir ses permissions.

Les notes historiques distinguent :
- les vertex input arrays légaux en GLSL3.30 et l'observation du nom pour size1 ;
- le retrait antérieur de quatre sondes de conformité fournisseur qui n'exerçaient pas le moteur ;
- les cas présents, dont deux exercent effectivement le Reader et le refus moteur ;
- le contrat Uniform size1 admis, qui ne doit pas hériter du refus Attribute.

Ces notes ne suffisent pas à imposer une orthographe native portable. Le spécialiste doit vérifier les sources Khronos versionnées et proposer, si nécessaire, une expérience minimale scalaire contre tableau mono-élément. Toute nouvelle source de métadonnées ou modification d'un contrat public sera examinée avec l'architecte avant auteur ; aucune décision de ce type n'est prise dans ce rapport.

## Suite autorisée

### Diagnostic reçu et expérience discriminante réservée

Le spécialiste rapporte une cause locale précise : `AttributeReader.hpp:103` déduit `isGLArray` exclusivement de `name.ends_with("[0]")`. Le nom nu observé avec size1 et type vec3 ne suffit donc pas à distinguer un scalaire d'un tableau mono-élément. Il propose de comparer les requêtes de localisation du nom nu et des éléments `[0]`/`[1]`. Cette proposition est **NOT_RUN** à cette étape, et le spécialiste ne garantit pas cette sémantique pour la baseline minimale3.3 : les règles qu'il cite sont explicitées en4.3/ARB_program_interface_query. Le lead lui a demandé les références exactes et les limites avant toute adoption.

Expérience autorisée au seul détenteur du bail Linux/GPU : trois petits programmes GLSL330 avec entrée active scalaire, array1 et array2 (contrôle utilisant les deux éléments). Enregistrer compilation/link, nom natif complet/size/type, réponses aux requêtes `qaProbe`, `qaProbe[0]`, `qaProbe[1]`, `missing`, erreursGL et contexte obtenu. Source et exécutable autonomes temporaires, sorties dédiées, outils déjà présents ; aucun changement du moteur, de CMake ou des tests de production. Ne pas compiler simultanément dans l'arbre de la campagne en cours.

Cette mesure vise une hypothèse, pas un verdict de conformité pilote. Elle ne remplace aucun test rouge ni ne démontre une solution universelle3.3. Si une nouvelle source de métadonnées devenait nécessaire, l'avis d'architecture et l'autorisation appropriée précéderaient l'implémentation.

Évaluation locale PM/LLM de cette source diagnostique exécutable : complexité brute55, retenue70 (plancher développement), risque40, incertitude liée à la sémantique minimale3.3. Propriétaire : producteur OpenGL/CI existant, aucune nouvelle admission ou politique de modèle. Plan QA existant O4/O8 ; témoins compile/link avant collecte, pas de RED/GREEN de correctif de production revendiqué.

La campagne ASan inclut toujours les trois tests, en sortie distincte. Une faute mémoire et un échec fonctionnel seront distingués ; aucune absence de diagnostic mémoire ne transforme un test fonctionnel rouge en réussite. Le workflow/helpers CI peut progresser indépendamment. Gaps, audit et clôture documentaire restent différés jusqu'aux entrées intégrées et vérifiées.

Pas de décision utilisateur supplémentaire demandée pour ce diagnostic borné. Si la seule solution exige une frontière architecturale hors mandat, la décision sera présentée avant implémentation.

## Annexe de traçabilité

- Producteur campagne : `task_mvsa43rc_0007_cq2xsm`, pool-5, build-engineer.
- Diagnostic lecture seule : `task_mvseouf3_0012_cb89ee`, pool-6, gpu-engineer ; PM/LLM C65/R68, pas une tâche de développement.
- Preuves Linux : `/home/artist/src/ARTIST-integration/out/linux-evidence/20261028T172225Z-4e352cedae774fe395f9ebf7ea6ccfef/` : opengl.xml, opengl-inventory.json et manifest.json.
- Lecture lead exécutée : Python standard XML/JSON dans cette distribution via Context Mode ; aucun build ni campagne GPU relancés.
- Extraits QMD : `references/session-2026-10-18-attribute-gl.md:37–77` et `architecture/missions/2026-10-19-opengl-lifetime-contracts.md:109–119`.
