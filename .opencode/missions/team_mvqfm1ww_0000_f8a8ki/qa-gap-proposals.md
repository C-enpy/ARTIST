# Propositions generator reçues — décision et mandat runner du lead

Auteur : test-engineer fresh, session `ses_e82683352ffeLTe6E71BlFTId4`.
Deux réponses finales natives complètes : `msg_17d9a1799001Uyd3gfEFfNE9Yz`
(10087 caractères) puis `msg_17d9b405b001CvPnBOlDvoHFUY` (3457 caractères).
Finish stop et completed présents, outcome succeeded. Le second texte remplace
la réserve d'identité du premier : HEAD dd2a2fa a la même source que H2 c283f80,
diff de trois documents uniquement, quinze empreintes corroborées par le lead.

Ensemble n'a pas retenu le final automatiquement et n'a pas complété la tâche.
Le lead a récupéré et lu les deux textes publics complets de la même session
via API native, sans relance, grant ou réparation du harness. Un glob initial
visait un dossier de mission inexistant ; le chemin local correct a ensuite été
trouvé et lu. Aucune panne de permission ou répétition d'un refus ne s'en déduit.
Profil natif restrictif, session sans override, contexte fresh, lecture locale et
messages reçus attestés ; pas de preuve de sandbox universelle.

## Décisions avant code

Le lead retient les trois propositions : un même fichier de test, aucune
modification de production permanente, fixture, framework ou oracle assoupli.
Les assertions et scénarios ont été relus contre le candidat identifié.

1. **KEEP — Révocation du pass avec uniform et témoin encore vivants.**
   Nouveau test `PassDestructionRevokesWithLiveUniformAndWitness` : créer owner
   Context id7 et réflexion scalar, préparer accès, retenir shared_ptr du témoin
   et sa génération, déplacer le wrapper uniform hors de la map et set(.25).
   Détruire le pass seul. Témoin n'accepte plus l'ancienne génération ; deux
   tentatives set(.5) doivent donner exactement STALE_PREPARED_ACCESS, sans nouvelle
   requête de contexte ni upload, valeur uniform conservée .25. Cela distingue
   la révocation du pass de la seule destruction de l'uniform.
2. **KEEP — Échec tardif de réflexion sans publication partielle.**
   Nouveau test `LateReflectionFailurePreservesPreparedPublication` : publier
   scalar location3 et écrire .25, retenir pointee/génération. Réflexion candidate
   de deux noms scalar identiques ; première entrée construite location4 puis
   seconde rejetée DUPLICATE_NAME. Génération, map unique, pointee et valeur
   précédents conservés ; ancien accès puis nouvelle préparation écrivent .5/.75
   à l'ancienne location3, trois uploads totaux. Le candidat rejeté n'est pas publié.
3. **MODIFY — Miroir conservé pour chaque rejet de contexte/programme.**
   Dans `ValueOnlyChangesRemainCompatibleAndWrongTargetDoesNotCommit`, ajouter
   get<GLfloat>()==.25 immédiatement après chacun des refus programme zéro,
   contexte absent et contexte incompatible, avant récupération .75. Aucun autre
   attendu n'est modifié ; l'état ne doit pas être silencieusement écrasé avant
   une récupération qui masquerait sa mutation.

Fichier : `ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp`.
Le patch texte proposé utilise helpers/symboles existants, pas de nouvelle fixture.
Compilation/exécution par le generator : NOT_RUN ; aucun défaut production reproduit.
Ces compléments ne remplacent ni le RED initial des auteurs ni une preuve historique
manquante. Réparations healer consommées : zéro sur deux au moment de réception.

### Application runner — réparation bornée 1/2

Le premier build lead a rejeté deux appels proposés à preparedUniformGeneration,
accesseur protected. Pas de test exécuté par ce build et pas de défaut production.
Le lead a remplacé ces lectures par le témoin public preparedUniformState().lock(),
sa generation() et accepts(generation) ; aucun changement de visibilité moteur.
Relance de la commande build Debug/unit : exit 0, 504 tests passants. Le JUnit
standard ancien copié après le premier build échoué est explicitement marqué
STALE-NOT-EXECUTED et exclu. Les ajouts sont donc deux tests, trois assertions
et cette adaptation test-only ; aucun oracle n'a été abaissé.

## Contrôles discriminants retenus, avant expériences

Préserver nominal puis expérimenter séparément, jamais cumuler les mutants.
Chaque expérience doit compiler et échouer pour son témoin, pas pour l'environnement.
Archiver diff/commande/log/identité, restaurer et vérifier hashes avant la suivante.

- **C, lookup CPU réel** : dans une copie expérimentale de PreparedFloatUniform,
  retenir temporairement un pointeur privé vers le pass reçu au constructeur ;
  après les gardes de set et avant le miroir, appeler réellement
  `getUniform("scalar")` via ce pointeur. Le test
  `RepeatedUpdatesObserveNoCpuNativeOrErasedResolution` doit détecter ΔC>0,
  ΔA=0 et trois uploads exacts. Ne pas simplement incrémenter le compteur.
- **L, résolution native** : même endroit, appel glGetUniformLocation au programme
  et nom scalar, résultat ignoré. L'attente unitaire Times(0) doit échouer ; variante
  native f2Scalar peut être utilisée pour vérifier l'espion réel, sans nécessité
  de nouvelle infrastructure ou nouvelle fonctionnalité.
- **A, extraction effacée** : même endroit, lire m_uniform->getValue<GLfloat>(),
  résultat ignoré. Le test doit détecter ΔA>0, ΔC=0 et trois uploads conservés ;
  sa calibration initialise déjà le miroir avant l'intervalle.

Les mutants sont jetables, non livrés, et ne reconstruisent pas un RED historique.
Ils restent INCONCLUSIVE avant exécution runner. Les tests pixel existants et les
deux contrôles négatifs restent KEEP ; pas de nouveau correctif ou test image demandé.

## Mandat runner distinct

Proposition : test-engineer. Persistance, relecture, application et exécutant réel :
lead. PM/LLM runner complexité73, risque74 ; patch test proposé complexité brute67
retenue70, risque68. Tests ajoutés avant exécution, aucune assertion affaiblie.
Défaut production : conserver rouge et retourner à l'owner. Deux réparations
healer au maximum, hypothèse et expérience nouvelles plutôt qu'essais identiques.
Ressources : un écrivain par fichier, un build/indexeur par sortie, GL sérialisé.
Revue C++ du snapshot de production puis mutants après sa fin pour ne pas changer
les entrées sous son analyse. Suites normales/ASan finales et audit fresh restent
nécessaires. Aucune nouvelle approbation utilisateur pour ce périmètre adopté.
