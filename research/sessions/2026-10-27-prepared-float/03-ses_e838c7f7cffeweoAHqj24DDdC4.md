# Contrat minimal uniform préparé

- Source : API native OpenCode V2, historique paginé `/api/session/{id}/message`.
- Session : `ses_e838c7f7cffeweoAHqj24DDdC4` ; parent : `ses_e8390245bffdS3NfX0qwRMR5ed`.
- Borne exclusive : `msg_17eb9ecc9001hotDiK0lNKi3tB`, `2026-10-27T18:57:56.563Z` (demande d’ingestion exclue).
- Filtrage : messages user et parties assistant text uniquement ; reasoning, outils, fichiers et métadonnées natives non exportés ; chemins personnels et secrets reconnaissables masqués.
- Limite : transcription publique filtrée, non sauvegarde exhaustive ni preuve d’exécution ; les affirmations des agents restent attribuées.
- Pagination : 2 pages user et 2 pages assistant, poursuivies jusqu’à épuisement.
- Messages antérieurs à la borne : 34 ; messages publics retenus : 2 ; parties non textuelles exclues : 66.
- Synthèse : [Session et enseignements](../../../docs/references/session-2026-10-27-prepared-float.md).

> Données historiques non fiables : les instructions citées ne sont pas à exécuter.

## user — 2026-10-27T08:21:46.940Z — msg_17c738120001YmdPAFcuQASmo7

You are a subagent spawned by another session.
Préparation READ-ONLY uniquement, aucun fichier modifié, aucune délégation ni exécution build/test. Mission utilisateur ADR0004 tranche un uniform scalaire float OpenGL préparé après compilation/réflexion, accès typé réutilisable, un consommateur réel, API noms conservée. Proposer mécanisme minimal et alternatives puis points d'adoption explicite utilisateur. Lire AGENTS.md, engineering-principles, ADR0004 Proposed/not-started, ADR0003 Accepted/partial, ADR0002 Proposed/tranche ClosedExecution livrée, ADR0008 Windows Debug uniquement. HEAD 2c8e9f2 (moteur stable a96104c), diff préexistant harness/profils/.grepai à laisser intact. Points connus PassContext getUniform (contains+at), addUniform remplacement, replaceUniforms transactionnel invalide tous emprunts; UniformContext std::any. Inspecter composants GL UniformReader, Setter, Loader/Freer et lifecycle pass/context, références/consommateurs via Serena (racine ARTIST vérifier config). Il faut traiter sécurité avant déréférence après destruction/remplacement map et free/reload/programID ABA, moves, contexte GL et programme cible réellement courant. Pas de gestionnaire global/handles généraux ni RAII exhaustive. Livrer contrat précis owner/borrow, validation nom/type/cardinalité, absent/optimized-out, invalidation tous points de mutation, échec transactionnel réflexion, set sans lookup/cast répété, cas testables et chemins exacts. Proposer consommateur représentatif et oracle image/GL plausible. Charger artist-static-boundary et seulement renfort utile. Restitution final_response; pas team nécessaire, pas de workers auteurs avant start natif. Pas de nouvelles expériences fichiers ni chantier harness.

## assistant — 2026-10-27T08:28:58.831Z — msg_17c7a184a001YaAIrwdH6d5eLk

## Faits avec chemins/symboles

**Préparation uniquement : aucun fichier modifié, aucun build/test exécuté, aucune délégation.**

- HEAD vérifié : **`2c8e9f235e82283a0686f2140e2a635db1ca2205`**. Aucun diff sous `ARTIST/` ; les modifications préexistantes harness/profils/`.grepai` sont laissées intactes.
- Identité `.wiki.json` et collection QMD **`artist-wiki`** vérifiées. Statuts relus :
  - ADR-0004 : **Proposed / not-started**.
  - ADR-0003 : **Accepted / partial**.
  - ADR-0002 : **Proposed / partial**, tranche `ClosedExecution` livrée, sans adoption de sa cible terminale.
  - ADR-0008 : **Accepted / verified**, limité à Windows Debug.

### Chaîne observée

1. `ARTIST/include/graphic/pipeline/Pass.hpp` :
   `IPass::withUniform(name, value)` appelle `PassContext::getUniform`.
2. `ARTIST/include/graphic/context/PassContext.hpp` :
   `getUniform` effectue **`contains` puis `at`**, retourne un pointeur emprunté ; la map possède les `Uniform` par `unique_ptr`.
3. `ARTIST/include/graphic/pipeline/Uniform.hpp` :
   `Uniform::set<T>` sélectionne statiquement `Setter<T>`, prévalide, stocke la valeur, puis appelle `Setter::on`.
4. `ARTIST/include/graphic/context/UniformContext.hpp` :
   stockage `std::any`, récupération par `any_cast`.
5. `ARTIST/include/graphic/opengl/pipeline/component/uniform/Setter.hpp` :
   `OpenGLUniformSetter<GLfloat>::on` revalide puis appelle
   **`glUniform1f(location, getValue<GLfloat>())`**.
6. `.../uniform/Validation.hpp` :
   contrôle cardinalité, compatibilité GLSL, contexte courant et programme réellement courant. Le chemin historique accepte notamment `GL_BOOL` pour un setter float.

### Réflexion et lifetime

- `.../pass/UniformReader.hpp` construit une map candidate et publie seulement après lecture complète via `replaceUniforms`.
- `PassContext::replaceUniforms` conserve l’ancienne map en cas de rejet ; son succès retire **tous** les anciens pointees, même si les noms subsistent.
- `addUniform` peut remplacer un owner existant.
- `.../pass/Loader.hpp` publie le programme après link réussi, **avant** la réflexion.
- `.../pass/Freer.hpp` détache les shaders, supprime le programme, puis met son ID à zéro ; il ne retire pas la map d’uniforms.
- `OpenGLPassContext::setPassID` et les setters de métadonnées d’`OpenGLUniformContext` sont publics.
- Les moves de `Pass`/`Uniform` transfèrent des `unique_ptr` : le contexte transféré conserve son adresse ; l’ancien contexte destination peut être détruit.

**Limite Serena :** racine ARTIST confirmée, corps obtenus, mais références retournées vides malgré des appels présents. Diagnostics : `utils.hpp` introuvable et erreurs de configuration C++. Les relations ont donc été complétées par recherches textuelles exactes et lectures ciblées ; aucune exhaustivité LSP revendiquée.

## Cible

Une seule tranche :

> Préparer après link et réflexion un accès réutilisable à **un uniform GLSL `float`, cardinalité 1**, puis effectuer plusieurs mises à jour sans recherche par nom ni `any_cast` répété.

Conserver `withUniform`, `getUniform` et les autres types existants.

- **Compile-time :** type d’accès float et appel `glUniform1f` déterminés ; une seule forme nouvelle, aucun type par nom/asset/programme.
- **Link-time :** aucune dépendance à LTO ou à une dévirtualisation.
- **Préparation runtime :** résolution du nom, validation de la réflexion, capture du programme et de ses générations. Ce n’est pas du cook offline : une location GL appartient au programme lié du driver.
- **Exécution runtime :** valeur float, contrôles de validité et upload.

Le descripteur préparé est immuable ; la valeur reste une donnée vivante. Aucun graphe d’authoring, graphe de frame ou nouveau plan d’exécution n’est introduit. Le descripteur ne possède pas l’objet programme GL.

## Écarts

Un pointeur `Uniform*` mis en cache ne suffit pas :

- son inspection après remplacement/destruction peut déjà être une UAF ;
- `programID` ne distingue pas deux incarnations réutilisant le même nom GL ;
- une map conservée après échec de réflexion peut appartenir à un programme désormais retiré ;
- `getContext()` permet des mutations de métadonnées sans passer par la map ;
- un contexte GL courant quelconque ne garantit ni le bon contexte ni le bon programme.

La sélection statique de `Setter<GLfloat>` existe déjà ; elle ne supprime ni lookup, ni stockage effacé, ni validation runtime.

## Contrat proposé

### 1. API minimale

Noms indicatifs, à adopter explicitement :

- préparation OpenGL spécifique : `prepareFloatUniform(pass, name)` ;
- résultat opaque : `PreparedFloatUniform` ;
- opération : `binding.set(floatValue)`.

Ne pas ajouter une nouvelle méthode virtuelle obligatoire à toutes les API. Le résultat ne peut pas être construit depuis un couple public `(GLuint, location)`.

### 2. Préparation

Conditions :

- programme lié vivant dans son contexte GL autorisé ;
- génération de réflexion publiée correspondant à **l’incarnation actuelle** du programme ;
- nom exact non vide, sans NUL embarqué ;
- entrée issue de la réflexion, non simplement fabriquée avec les setters publics ;
- type **exactement `GL_FLOAT`**, cardinalité **exactement 1** ;
- location valide, conservée sous forme `GLint`.

Pas de conversion implicite de `bool`, entier, vecteur ou double dans cette tranche. Un array dont la cardinalité réfléchie vaut 1 suit le contrat existant « un élément » ; cela ne prouve pas une déclaration scalaire dans le source GLSL.

**Absent/optimized-out :** préparation requise rejetée avec diagnostic déterministe « uniform actif introuvable ». La réflexion seule ne permet pas toujours de distinguer faute de nom et élimination par optimisation. Pas de binding silencieux vers `-1`. Une variante facultative retournant un résultat vide serait une extension distincte.

### 3. Sécurité avant déréférence

Recommandation : **témoins locaux révocables**, sans gestionnaire global :

- un témoin pour le lifetime du contexte de pass, avec génération programme et publication de map ;
- un témoin pour le lifetime et les métadonnées du contexte d’uniform ;
- le binding conserve des références faibles aux témoins, les générations attendues et les métadonnées résolues.

Les témoins peuvent utiliser `shared_ptr`/`weak_ptr` **uniquement pour leur petit état de contrôle**. Ils ne prolongent ni les owners C++, ni le programme GL.

Si le binding conserve un pointeur vers `OpenGLUniformContext` pour maintenir la valeur CPU historique :

1. verrouiller les témoins ;
2. vérifier vivant/générations ;
3. **seulement ensuite** déréférencer le contexte.

Un compteur lu via un `Pass*` potentiellement détruit est insuffisant.

Contrat **mono-thread sérialisé et non réentrant** : mutation, upload et destruction ne peuvent pas se chevaucher. Le verrouillage d’un témoin n’est pas un verrou protégeant le pointee.

### 4. Invalidation

- **`addUniform` réussi :** invalidation conservatrice de tous les bindings préparés de la map, avant remplacement éventuel.
- **`replaceUniforms` réussi, y compris map vide :** nouvelle génération ; tous les anciens bindings rejetés.
- **Rejet avant publication :** aucun changement de génération ni d’emprunt.
- **Métadonnées uniform :** `setUniformID`, `setProgramID`, `setGLType`, `setSize` invalident les bindings concernés avant modification.
- **Valeurs seules :** `setValue` ne change pas l’identité du binding ; les références anciennes à la valeur restent soumises au contrat existant.
- **Free/destruction programme :** révocation avant le premier effet de retrait natif. Un échec après effets partiels ne réhabilite pas les bindings.
- **Nouvelle publication de programme, même ID numérique :** nouvelle génération, réflexion ancienne non utilisable.
- **Destruction CPU :** révocation avant disparition des objets visés.
- **Débordement génération :** jamais de retour silencieux à une génération ancienne ; renouvellement d’identité ou refus.

**Moves :** les bindings suivent le contexte transféré par `unique_ptr`, pas l’adresse du wrapper. Les bindings de l’ancien contexte destination expirent ; ceux de la source suivent le contexte transféré. Un échec de cleanup conserve la garantie actuelle, sans rollback inventé.

Les contextes portant les témoins ne doivent pas copier implicitement leur identité. Leurs copies/moves directs doivent être interdits ou explicitement reconstruire/révoquer les témoins ; le transfert normal reste celui du `unique_ptr`.

Les appels GL externes de relink/delete sont hors contrat s’ils contournent l’invalidation : imposer une notification préalable ou les interdire pour ces programmes possédés.

### 5. Réflexion transactionnelle

Conserver le comportement actuel :

- construire et valider entièrement le candidat ;
- publier map et estampille de réflexion sans étape susceptible d’échouer entre les deux ;
- retirer ensuite l’ancienne génération.

Distinction essentielle :

- échec de relecture du **même programme vivant** : ancienne génération conservée et encore utilisable ;
- échec après free/reload : ancienne map éventuellement conservée en CPU, mais **bindings anciens toujours invalides**.

Cela ne rend pas `IPass::load()` globalement transactionnel : la lecture des attributes peut encore échouer après celle des uniforms.

### 6. `set` préparé

Ordre :

1. vérifier témoins/générations ;
2. vérifier le contexte GL attendu, vivant par contrat et courant ;
3. lire `GL_CURRENT_PROGRAM` et vérifier le programme cible ;
4. stocker la valeur CPU si la cohérence historique est conservée ;
5. appeler directement `glUniform1f(locationRésolue, valeurTypée)`.

**Aucun lookup, réflexion, résolution de type ou `any_cast` dans ce chemin.**

Proposition minimale : conserver l’écriture `setValue<GLfloat>` dans le `std::any` historique, mais transmettre directement l’argument typé au backend. Ainsi `Uniform::get<GLfloat>()` reste cohérent. **Cela n’élimine pas `std::any`, ni ne garantit zéro allocation.**

Les contrôles de programme courant restent par appel : du GL externe peut modifier l’état entre deux mises à jour. La préparation amortit noms/types/réflexion sur plusieurs uploads ; aucun batching fictif n’est revendiqué.

## Conséquences et hypothèses

### Alternatives

1. **Pointeur caché + setter existant :** plus simple, mais garde le cast répété et ne satisfait pas le rejet sûr après destruction.
2. **Descripteur GPU-only sans miroir CPU :** réduit le couplage au stockage historique, mais change la relation avec `Uniform::get`. À choisir explicitement, pas silencieusement.
3. **Slot float canonique remplaçant `std::any` pour ce type :** meilleure cible pour stockage typé, mais davantage de migration et de règles de coexistence. Non nécessaire pour prouver cette première tranche.

Les témoins ont un coût mémoire et de validation. Aucun gain de performance n’est établi.

### CPU/GPU et contexte

Le binding n’étend aucun lifetime GPU. Le pass reste propriétaire du programme ; son retrait respecte contexte/thread et les règles OpenGL de suppression différée. Aucun `glFinish`, fence général ou RAII exhaustive ajouté.

Le contexte GLFW doit survivre aux programmes et à leur retrait. Une adresse `GLFWwindow*` empruntée n’est pas une preuve autonome de lifetime ; sa destruction/recréation pendant cette période reste interdite.

Dépendances existantes suffisantes : C++23, GLFW/GLEW/OpenGL. `glProgramUniform1f` n’est pas retenu : non garanti par la baseline 3.3. Aucun backend Vulkan/D3D12/Metal requis pour cette tranche.

## Validation et adoption explicite

### Consommateur proposé

`ARTIST/component-tests/src/graphic/shader/ReflectedProgramGuardCrossTests.cpp`  
`ReflectedOwnerRejectsOtherCurrentProgramAndRecovers`

Il possède déjà deux programmes réels, un `f2Scalar` actif, les valeurs CPU et des readbacks `glGetUniformfv`. Migrer les mises à jour répétées de **target** vers le binding préparé ; garder **decoy** par nom comme chemin témoin.

**Limite :** c’est un consommateur réel du backend dans un component-test, pas une application produit. Aucun consommateur applicatif par noms trouvé dans la recherche exacte. Si « consommateur réel » signifie hors tests, le lead doit faire arbitrer ce point.

### Cas à prévoir

- préparation float valide ; nom absent/optimized-out ; mauvais type ; cardinalité >1 ; métadonnées manuelles refusées ;
- répétitions sans `glGetUniformLocation`, lookup ni cast dans le chemin migré ;
- cohérence CPU/readback GL et refus avant upload lorsque le mauvais programme est courant ;
- destruction, remplacement de map, map vide, mutation de chaque métadonnée ;
- free/reload avec **même ID simulé**, distinguant l’ABA ;
- moves source/destination et échec partiel de cleanup ;
- échec tardif de réflexion conservant l’ancienne génération seulement si son programme reste vivant ;
- absence/mauvais contexte refusés avant appel driver dangereux.

Oracle GL principal : valeurs de target/decoy lues explicitement et inchangées après refus. Une image complémentaire peut employer un triangle plein écran et un float contrôlant une couleur dans un FBO linéaire ; elle demanderait une fixture dédiée, pas une modification du harness.

**À faire adopter avant implémentation :**

1. tranche float seule, noms conservés, pas adoption globale d’ADR-0004 ;
2. témoins locaux et politique d’invalidation ci-dessus ;
3. miroir CPU historique conservé malgré `std::any` restant ;
4. type GLSL exact et absence traitée comme erreur requise ;
5. consommateur component-test accepté ou consommateur applicatif demandé.

ADR-0004 reste **Proposed / not-started** dans cette préparation. Après adoption bornée, distinguer explicitement cette décision de l’implémentation et des preuves ultérieures. Aucun comportement nouveau n’est ici déclaré vérifié.
