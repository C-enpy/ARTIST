# Complément generator F1/F2 — propositions reçues et retenues avant code

Auteur test-engineer fresh : session `ses_e824039f2ffeU9dNg3WyIL1eV5`, profil
natif restrictif/OpenAI high/permissions de session[] contrôlés, lectures relatives
et communication reçues. Finale publique `msg_17dc3717c001rH2QUeETm3rSLD`,
9563 caractères, stop/completed ; finale suivante
`msg_17dc44e2d001HPuyFsUgey0VBB` confirme inchangé. Outcome succeeded.
Ensemble n'a pas retenu la finale ; le lead a lu les deux textes complets via
l'API native de la même session. Aucun worker relancé, grant ou réparation.

## Décision du lead

Les deux scénarios et leur patch texte sont KEEP, après relecture contre la fixture,
le journal GL et le scénario partiel ShaderLifetime existant. Seul fichier modifié :
`ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp`. Includes shader
et validator voisins ajoutés pour instancier les composants existants, aucun helper
global, nouveau fichier de test, mock, CMake ou header de production modifié.

### F1 — Préparation sous mauvais contexte

Nouveau test `PreparationRejectsMissingAndOtherContextBeforeNativeQueriesAndRecovers`.
Réflexion scalar valide, miroir .25, témoin public et génération conservés.
Deux contextes invalides : nullptr puis autre pointeur. Une optional d'accès doit
rester vide ; diagnostics exacts NO_CURRENT_CONTEXT puis WRONG_CONTEXT. Nombre
de GL calls inchangé pendant le négatif, zéro upload, pointee/mirror/témoin intacts.
Conserver le lien valide configuré par la fixture pour que la suppression de la
garde ne soit pas masquée par PROGRAM_NOT_LINKED. Restaurer contexte propriétaire,
préparer puis set .75, location3 et payload exact. Mutant à exécuter séparément :
supprimer requireContext dans prepare, pas dans set.

### F2 — Révocation avant premier détachement puis échec partiel

Nouveau test `PartialDetachFailureRetiresBeforeFirstEffectAndRejectsRepeatedSets`.
Pass owner id7, deux shaders61/62, réflexion scalar et accès préparé, set .25.
Déplacer le second wrapper vers parked ; la boucle Freer détache le premier puis
rejette le second moved-from. À l'appel glDetachShader(7,61), vérifier le témoin
déjà révoqué sans réentrer l'accès. Aucun detach62/deleteProgram/deleteShader
pendant l'échec. Restaurer le wrapper avant assertions/destruction ; exactement
un GL call ajouté, programme encore7, pointee/mirror intacts. Deux set .5 refusés
STALE_PREPARED_ACCESS, sans query/call/upload nouveau. Nettoyer les attentes puis
attendre cleanup detach61/detach62/deleteProgram7 et deleteShader61/62.
Mutant distinct : déplacer retirePreparedUniforms après la boucle de détachement.

## Mandat runner

Propositions : generator ; relecture, persistance, application, exécution : lead.
Aucun test/RED/GREEN exécuté par la QA, aucun défaut production reproduit.
Ces nouveaux tests couvrent deux frontières du contrat courant, pas un RED
historique reconstruit. Vérifier les deux tests au nominal, exécuter les deux
mutants séparément après compilation, garder échec causal/restauration/empreintes,
puis renouveler contrôles normaux/ASan et ré-auditer la session indépendante.

Le journal GL intercepte tous les appels, dont glGetProgramiv. Les accesseurs de
génération sont lus via preparedUniformState().lock(), jamais par l'accesseur
protected. Healer : 1/2 déjà consommée avant ce complément, aucune nouvelle
réparation consommée à réception. Toute panne production reste rouge et revient
à son owner ; pas d'oracle assoupli ni d'extension d'infrastructure.
