# pool-16 (@engine-architect teammate)

Session: ses_e74f4e5a4ffeQ7njXL8npBS6VF
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T04:23:50.666Z — msg_18b0cbe87001YPanl3sVvotoPr

# Candidat lecture seule — deux coutures Windows

**Recommandation :** exécution directe par le runner commun ; transport privé prepare/build matérialisé par le sink, avec **un seul argument explicite `bundle_root` ajouté à `describe_preparation`**. À ratifier par le lead avant écriture.

Aucun fichier modifié, aucun test, build, workload ou outil Windows exécuté. Aucun outil d’équipe utilisé.

## 1. Faits vérifiés

### Contrats et documentation

- `.wiki.json` désigne bien `docs/` et la collection `artist-wiki`.
- Lecture de `docs/adr/README.md`, `docs/architecture/correction-plan.md`, `docs/engineering-principles.md` et chargement d’`artist-static-boundary`.
- Recherche QMD dans `artist-wiki`, puis lecture d’ADR-0008 : **Accepted/verified limité à Windows Debug**, pas qualification générale Windows/Linux.
- ADR-0001/2/4/6/7 restent Proposed ; 0003/5 Accepted/partial. Ces coutures n’adoptent pas leurs cibles globales.

### Chaîne actuelle

Au canon consulté :

- `scripts/verify.py::_adapter` sélectionne une classe dans une table fermée et construit `WindowsAdapter(source_root)`.
- `adapters/base.py::PlatformAdapter.describe_preparation(plan)` ne reçoit aucun emplacement de preuve.
- `evidence.prepare` crée le bundle, prend son lease et capture identité/environnement **avant** `describe_preparation`.
- `evidence.build` admet le receipt et l’identité de préparation avant `describe_build`.
- `evidence.execute` admet le BuildReceipt et son snapshot avant `describe_execution`.
- `evidence._execution_requests` inspecte les véritables `CommandSpec.argv` : population scénario/route, paramètres, identifiants et sorties uniques.
- `evidence._build_files` exige une correspondance unique et non vide pour chaque cible ; `BuildReceipt.details.executables` contient ses chemins relatifs à la source.
- `EvidenceSink.command` conserve argv, effets, code natif et journaux stdout/stderr ; `SubprocessRunner` réalise déjà la capture commune.
- `runner.py` retire les six variables publiques de contexte ambiantes avant application de `env_delta`. Cela **ne constitue pas un filtrage intégral de l’environnement hérité**.

Dans le worktree Windows :

- Le plan `scripts/tests/verification_windows/WINDOWS_ADAPTER_READONLY_PLAN.md` identifie correctement les deux coutures.
- `build-windows.ps1::Resolve-VerificationInputs` attend exactement quatre champs privés : `build_directory`, `targets`, `executable`, `arguments`.
- Pour prepare/build, les deux derniers doivent être respectivement `""` et `[]`.
- `Resolve-VerificationPhaseFlags` exige InputFile/BundleRoot/logs et réserve l’opt-in dépendances à prepare.
- La région de dispatch prévoit Conan avant CMake et l’arbre `windows-msvc-{configuration}-verification`.
- **Le câblage réel reste fermé** : la garde précédant cette région rejette actuellement `VerificationPhase`. Les fonctions natives et fixtures présentes ne prouvent donc pas un adaptateur exécutable intégré.

## 2. Cible et couture 1 — exécution directe

### Contrat proposé

`WindowsAdapter.describe_execution(plan, build)` produit :

- pour chaque workload, un `CommandSpec` dont `argv[0]` est l’exécutable correspondant au **BuildReceipt admis** ;
- toutes les options réellement transmises au workload dans ce même argv : scénario, route, paramètres, run/request/plan, résultat et options GL applicables ;
- pour CTest, les commandes explicites de découverte et de sélection/exécution, compatibles avec le collecteur intégré.

Aucun payload PowerShell de run, argv miroir, base64, second lancement ou wrapper opaque.

L’adaptateur résout les références du receipt sous la racine source et les rapproche de son inventaire d’identité. Il ne transforme pas une concaténation `builddir/target.exe` en autorisation.

**CTest est l’outil lanceur, pas une cible du BuildReceipt.** Les exécutables moteur qu’il sélectionne doivent correspondre aux cibles admises ; découverte et exécution doivent utiliser le même arbre, la même configuration et la sélection fermée prévue.

### Responsabilités

- **Core :** admission, contrôle de population, capture, journaux, résultats et agrégation.
- **Windows :** assemblage pur des commandes et environnement enfant explicitement nécessaire.
- **Collecteur contexte/JUnit :** propriétaire distinct ; aucune substitution locale.

L’environnement obtenu dans un processus PowerShell ne devient pas celui du parent Python. Le run direct doit consommer le mécanisme d’environnement Windows effectivement intégré et approuvé, sans nouvel export global ni dump de secrets. Le diagnostic ENV Windows demeure hors de cette remise.

### Sort du helper

- Conserver sa voie **prepare/build**, après câblage et validation dédiés.
- Ne pas ouvrir sa voie run pour satisfaire l’adaptateur commun.
- Les fonctions privées de run/capture déjà présentes peuvent rester **source-only, hors critère de réception du run**, sans suppression spéculative.
- La capture PowerShell privée ne doit pas envelopper les workloads exécutés par `SubprocessRunner`, ni fournir une seconde autorité de logs.

Cette résolution se fait au **runtime de qualification**, une fois les exécutables et leur identité connus. Aucun changement compile-time, link-time ou cook-time du moteur n’est nécessaire.

## 3. Couture 2 — transport de préparation explicite

### Pourquoi la signature actuelle ne suffit pas

À plan et racine source identiques, deux préparations peuvent posséder deux bundles différents. Une fonction pure `describe_preparation(plan)` ne peut pas choisir leur InputFile correctement sans :

- recevoir cette information ;
- la deviner ;
- consulter un état caché ;
- ou introduire une matérialisation différée supplémentaire.

Les trois dernières solutions sont moins explicites ou plus complexes.

### Recommandation minimale

Modifier uniquement cette signature :

```python
describe_preparation(plan, *, bundle_root: str) -> tuple[CommandSpec, ...]
```

`bundle_root` est un chemin absolu fourni par `evidence.prepare` après création et admission du bundle. Ce n’est ni un sink, ni un objet vivant caché, ni un nouveau modèle sérialisé.

**Pas de `run_id` supplémentaire nécessaire** pour le transport actuel : le payload ne le contient pas ; l’enveloppe de preuve conserve déjà run_id et plan_sha256.

Les implémentations Linux et les doubles acceptent également ce keyword explicite. Aucun fallback silencieux par interception de `TypeError`.

### Matérialisation

Le **propriétaire core**, dans une petite routine privée bornée Windows, effectue sous lease :

1. Vérification de l’opt-in avant toute commande potentiellement résolutive.
2. Création exclusive d’un fichier privé à emplacement déterministe dans le bundle, par exemple `raw/windows-helper-input.json`.
3. Écriture des quatre champs existants :
   - `build_directory` : règle canonique `_build_root` ;
   - `targets` : union ordonnée et dédupliquée des cibles du plan ;
   - `executable: ""` ;
   - `arguments: []`.
4. Fermeture du fichier, puis empreinte et inscription via `sink.artifact`.
5. Création exclusive du répertoire privé de logs prepare.
6. Appel de la description pure avec `bundle_root`, puis exécution des commandes déclarées.

Le nom exact des chemins privés est fixé une fois lors de l’intégration. Aucune recherche de « dernier bundle ».

L’adaptateur ne rédige pas le payload : il décrit seulement une invocation PowerShell utilisant les chemins explicitement liés au bundle. Cette petite dépendance Windows dans la matérialisation du core est volontairement bornée aux quatre champs existants ; elle ne devient pas un mini-DSL de `CommandSpec`.

### Build et no-clobber

- Le build réutilise le payload de préparation, identique pour ces quatre champs, **après admission et vérification de son empreinte**.
- Son bundle est déjà disponible dans le PreparationReceipt ; aucune nouvelle signature de build n’est nécessaire.
- Le sink crée un répertoire de logs build distinct sous son lease.
- Une collision ou un transport modifié est un échec explicite, jamais une autorisation de réécriture.
- Une préparation déjà commencée n’est pas « réparée » en écrasant son fichier ; nouvelle tentative = nouveau bundle.
- Les payloads et journaux partiels restent conservés après échec. Pas de suppression automatique qui détruirait la preuve.

Le payload reste au format privé du helper : **ne pas lui ajouter l’enveloppe JSON publique**, puisque son lecteur refuse les champs supplémentaires. Le manifeste fournit la liaison run/plan/empreinte.

### Alternatives

- **Sans changement d’interface :** possible seulement en supprimant le transport fichier du helper ou en ajoutant un mécanisme de liaison différée. Cela déplacerait davantage de responsabilités ; non recommandé.
- **Constructeur avec bundle mutable, setter ou variable globale :** rejeté, car état caché et risque de réutilisation inter-run.
- **Contexte/receipt supplémentaire ou description générale d’artefacts :** disproportionné pour cette couture.
- **Trois keywords InputFile/BundleRoot/logs :** explicite, mais inutilement large puisque les deux chemins privés sont dérivables du bundle selon une convention unique.

## 4. Diff de contrat minimal à ratifier

Ajouter aux sections 4 et 6 du contrat :

> `describe_preparation(plan, *, bundle_root: str)` reste pur. Le core fournit le bundle effectivement possédé après sa création et sous lease. Pour Windows, le sink matérialise et empreinte le transport privé borné avant invocation ; l’adaptateur ne réalise aucune écriture. Request, CommandSpec et receipts restent inchangés.

> Le run Windows décrit directement les workloads admis et CTest. Les options inspectées sont celles réellement exécutées. Le runner commun possède la capture ; le helper privé de run ne participe pas au chemin de qualification.

Préserver explicitement :

- Conan 2 avant CMake ; arbre/configuration/toolchain cohérents ;
- opt-in prepare uniquement, aucun bootstrap build/run ;
- absence d’effets pour list/plan ;
- absence de nouveau backend ou de repli logiciel ;
- états pipeline/test/mesure distincts.

## 5. Propriétaires et ordre d’intégration

1. **Lead :** ratification et point Git coordonné.
2. **Intégrer le producteur contexte pool-14**, puis vérifier les interfaces réellement obtenues.
3. **Un seul propriétaire core** modifie `contract.md`, `adapters/base.py`, `evidence.py` et les doubles communs : keyword explicite et matérialisation.
4. **Auteur Linux**, après transfert coordonné, adapte sa signature sans changement de comportement.
5. **Auteur Windows** implémente `windows.py` et le câblage prepare/build du helper sur cette base intégrée.
6. Lead intègre, puis observe les validations autorisées et organise la revue ciblée.

Pas d’écriture concurrente avec pool-14 sur `evidence.py` ; pas de copie de son collecteur depuis une branche isolée. Le collecteur JUnit et le diagnostic ENV restent à leurs auteurs.

## 6. Validation discriminante proposée

**RED causal, puis GREEN et non-régression, exclusivement synthétiques :**

- list/plan : factory, écriture, sonde et subprocess interdits ; aucun payload créé ;
- deux bundles du même plan : chemins distincts, aucun état retenu entre descriptions ;
- description pure : aucune création de fichier ;
- le fake runner constate que le payload est fermé, conforme et empreinté **avant** son appel ;
- collision de payload/logs, modification du payload entre prepare/build, chemin sortant : refus avant lancement ;
- opt-in absent : aucun appel Conan/CMake ; build/run ne transportent jamais l’autorisation de bootstrap ;
- configuration debug/release et cible CPU/CTest : racine correcte, cibles exactes, Conan avant CMake, paramètres de découverte préservés ;
- receipt incompatible, exécutable absent/ambigu/modifié ou mauvaise association cible : zéro lancement ;
- workload direct : options visibles identiques aux options réellement reçues, résultat unique, aucune commande helper-run ;
- capture : un seul passage par le runner commun ; codes, timeout, interruption et logs disponibles conservés ;
- CTest : découverte puis sélection concordantes sur données factices ; aucune population vide favorable ;
- variables publiques ambiantes : ne deviennent pas des déclarations de contexte ;
- code enfant zéro sans résultat conforme : jamais PASS.

Les commandes précises attendent les fichiers intégrés ; les suites existantes `verification_core` et `verification_windows` seront utilisées sans infrastructure supplémentaire.

## Conséquences et limites

La frontière dynamique est **une commande par workload ou opération CTest**, pas par update GPU. Le catalogue reste fermé ; aucune variante C++ supplémentaire ni coût d’instanciation ajouté.

Le plan est une description immuable ; le bundle est une preuve persistante possédée par le sink. Ils ne sont ni graphe de frame ni objet driver. Les owners et durées de vie GPU restent ceux des workloads existants : ces coutures n’ajoutent aucune garantie de destruction en vol.

**État : propositions non implémentées, à ratifier.** Les lectures confirment les incompatibilités structurelles, pas un échec runtime reproduit. Aucun résultat de qualification Windows/Linux, de rendu ou de performance n’est acquis ici ; les fonctions MSVC source-only ne constituent pas une preuve de device.

## assistant — 2026-10-30T06:41:23.697Z — msg_18b8aaced001zbAaGlXN44rWZr

## Décision proposée au lead

**Ratifier cette couture privée bornée**, sans rouvrir le binding déjà intégré : utiliser les deux entrées du `CMakeCache.txt` du **même arbre admis** pour décrire CTest par chemin absolu.

Lecture effectuée : `BINDING_RED_READY.md:86–129` dans le worktree pool-23. Aucun edit, test, processus ou outil natif exécuté.

### Contrat minimal

Dans `WindowsAdapter.describe_execution` :

1. Déterminer le cache exclusivement depuis l’arbre du BuildReceipt admis ; exiger son entrée dans `details.identity.files`. Aucun cache historique, recherche PATH ou venv supposé.
2. **Lire les octets une seule fois**, comparer leur SHA-256 au receipt, puis décoder/parser **ces mêmes octets**. Corriger ainsi la formulation « vérifier le digest avant lecture » : elle ne doit pas conduire à deux lectures indépendantes.
3. Exiger exactement une définition de chaque clé :
   - `CMAKE_COMMAND:INTERNAL`
   - `CMAKE_CTEST_COMMAND:INTERNAL`
4. Refuser clé absente, doublon même identique, mauvais type, définition malformée ou valeur vide. Une occurrence avec un autre type ne doit pas être ignorée comme si elle n’existait pas.
5. Exiger des chemins Windows absolus, sans expansion d’environnement ni interprétation shell, désignant respectivement `cmake.exe` et `ctest.exe` dans **le même répertoire d’installation normalisé**. Comparaison selon la sémantique Windows, indépendante de l’hôte des tests synthétiques ; pas de résolution via cwd ou PATH.
6. Employer ce chemin CTest inchangé pour découverte et exécution, avec configuration, arbre et sélection concordants.

Lecture de données d’identité admises seulement : **aucune écriture, sonde de version, lancement, installation ou mutation d’environnement**. Cette pureté opérationnelle est compatible avec la proposition ; ce n’est pas une fonction indépendante du filesystem.

Le cache n’est consulté que pour les scénarios CTest. Un plan workloads-only ne doit pas acquérir artificiellement cette dépendance.

### Identité : limite importante

Le hash du cache garantit **l’identité des octets qui déclarent le chemin**, pas celle du binaire CTest à cet emplacement, ni son équivalence fonctionnelle avec CMake.

Le contrôle « même répertoire » est un invariant de cohérence, **pas une attestation cryptographique d’installation**. Un remplacement ultérieur de `ctest.exe` reste possible. Les traces helper déjà capturées restent des observations attribuées à leur phase, sans devenir automatiquement une admission du binaire au run.

Ne pas ajouter ici de sonde ou de nouveau schéma pour combler cette limite. Un outil absent ou impossible à lancer produit une panne explicite du pipeline, jamais un fallback. La qualification native demeure **NON_EXECUTE**.

### Tests synthétiques minimaux avant GREEN

- Nominal : cache empreinté, deux entrées valides ; chemin absolu attendu dans les deux commandes CTest.
- Cache absent, absent de l’inventaire ou digest divergent : refus avant runner.
- Chaque clé absente, doublée, mal typée, malformée ou vide : refus.
- Chemin relatif, mauvais nom d’exécutable ou installations divergentes : refus.
- Chemins Windows avec espaces et variantes de séparateurs/casse : comportement déterministe, sans shell.
- Une seule lecture : digest et parsing utilisent exactement le même buffer.
- Workloads-only : aucune lecture de cache requise.
- Pureté : zéro écriture/processus/sonde ; aucun fallback PATH.
- Binaire outil remplacé à chemin constant : **aucune prétention de détection par le hash du cache**.

### Intégration et alternative

Ownership : auteur Windows, fichiers Windows uniquement ; contrats publics, core, presets et C++ inchangés. Ratification lead → négatifs RED supplémentaires → implémentation minimale → GREEN/non-régression par le lead.

L’alternative `ctest` via PATH est plus courte mais ambiguë ; un registre d’outils serait disproportionné. Cette couture est donc la plus petite compatible avec les exigences observées, **sans preuve native ni qualification du binaire outil lui-même**.

## assistant — 2026-10-30T06:54:18.564Z — msg_18b967fc00018E6o1D9xjb3Ly7

## Décision proposée au lead

**Ratifier une correction privée du runner, déclenchée uniquement par une déclaration hardware explicite.** Ne pas généraliser à tout contexte déclaré.

### Fait vérifié

`scripts/verification/runner.py:44–47` copie l’environnement parent, retire seulement `CONTEXT_ENV_NAMES`, puis applique `env_delta`. Un enfant hardware peut donc actuellement hériter de `LIBGL_ALWAYS_SOFTWARE=true` ou `GALLIUM_DRIVER=llvmpipe`.

### Contrat minimal

Pour chaque `CommandSpec`, sans inspection de l’environnement dans les descriptions :

1. Copier l’environnement parent, sans le modifier.
2. Retirer les six variables publiques de contexte ambiantes, comme actuellement.
3. Si **`command.env_delta['ARTIST_VERIFICATION_CONTEXT_V1'] == 'opengl-hardware'`** :
   - exiger les six champs explicites de transport, non vides ;
   - retirer de la copie enfant `LIBGL_ALWAYS_SOFTWARE` et `GALLIUM_DRIVER` ;
   - **refuser avant lancement** leur présence dans `env_delta`, plutôt que permettre leur réintroduction.
4. Appliquer `env_delta`, puis lancer normalement.

La validation sémantique des identités et de la cible reste au core ; le runner ne devient pas un second parser de receipt.

**Portée : workloads et CTest**, y compris les commandes CTest de découverte portant cette déclaration. Aucun traitement basé sur argv, PATH, nom de GPU ou environnement hardware ambiant.

Pour software, l’adaptateur conserve ses déclarations explicites de forçage logiciel. Pour CPU ou commande sans déclaration hardware, **héritage inchangé par rapport au runner actuel**, notamment pour ces deux variables. Aucun `None`, valeur vide artificielle ou changement de schéma.

### Pourquoi ce choix

- Le binding réel de l’environnement enfant appartient au runner, au moment du lancement.
- Nettoyer tous les contextes élargirait inutilement le comportement software/CPU.
- Refuser tout parent contaminé serait compatible, mais moins utile : la copie enfant peut être assainie sans modifier le parent ni perdre de preuve.
- L’assainissement réalise le profil demandé ; ce n’est pas un fallback. Il **ne prouve pas** qu’un renderer matériel sera obtenu : les gardes GL et la cible attendue restent obligatoires.

### Tests discriminants synthétiques

Avec un `popen_factory` factice inspectant l’environnement reçu :

- hardware explicite complet + parent logiciel : les deux variables sont absentes chez l’enfant ;
- même propriété pour workload et CTest, sans dépendance au contenu d’argv ;
- hardware incomplet : refus avant `Popen` ;
- hardware réintroduisant une des deux variables par `env_delta` : refus avant `Popen`, quelle que soit sa valeur ;
- contexte hardware seulement ambiant : aucun déclenchement ; six variables ambiantes toujours nettoyées ;
- software explicite : ses deux valeurs déclarées sont transmises ;
- CPU/sans déclaration : héritage des deux variables inchangé ;
- environnement parent strictement préservé ; deux commandes successives hardware/software ne se contaminent pas.

### Ownership et limites

Modification commune nécessaire : **`runner.py` et tests core**, sous propriétaire explicitement transféré par le lead. L’auteur Linux peut préparer ses tests d’intégration, sans écrire le runner en parallèle.

Aucune édition ni exécution effectuée ici. Recommandation à ratifier avant GREEN ; aucune qualification GPU/native ni garantie contre d’autres mécanismes externes de sélection du driver.

## assistant — 2026-10-30T08:34:32.273Z — msg_18bf242cc001WLGVyuZBNyXzdm

## Avis au lead — ratification locale conditionnelle

**La proposition est compatible, mais pas encore minimale ni complètement raccordée aux preuves.** Retenir le contexte privé, un petit témoin de préparation et le collecteur core borné. **Le collecteur est nécessaire avant réception du helper** ; sa responsabilité doit être transférée explicitement.

Aucune modification ou exécution effectuée. Lectures : contrat §6, binding Windows v1, QA §13, wrappers/helper actuels, `evidence.py:269–373` et note pool-31.

### 1. Constats et cible

- `Invoke-VerificationProcessLeaf(Program, CommandLine)` ne transporte pas cwd/env/logs : préserver sa signature impose un contexte privé accessible au binding.
- `_windows_helper_logs` crée les répertoires, mais `BundleSink.command` n’inventorie que les deux logs communs. Les sorties privées actuelles ne deviennent donc **pas automatiquement des artefacts du receipt**.
- QA §13 impose admission avant effets, séparation prepare/build, première cause, conservation des bruts et pins inchangés. Le RED fonctionnel communiqué par le lead ne couvre pas encore cette nouvelle composition.

### 2. Contexte privé par invocation : acceptable

Le contexte contient uniquement :

- racine dérivée du vrai `PSScriptRoot`, bundle/arbre admis, phase/configuration ;
- map d’environnement **destinée aux enfants**, chemins absolus des outils ;
- répertoire de logs possédé, index de commande et échéance globale.

Il est créé **après admission**, non réentrant, inaccessible hors de l’invocation et retiré en `finally`. Les feuilles refusent son absence ; aucun fallback vers le legacy, l’environnement OS ou un contexte précédent.

`Read/Write-VerificationEnvironmentLeaf` opèrent sur cette map, jamais sur l’environnement du processus. Les wrappers et primitives natives restent pinés. Un unique objet Process est possédé par chaque lancement, avec libération dans le binding ; cela **ne clôt pas le gap historique Dispose/writers hérités**.

Le budget restant doit borner chaque commande : un nouveau timeout complet par sous-commande ne doit pas multiplier silencieusement le budget du helper.

### 3. MSVC et export filtré

Accepter le choix proposé :

- VS explicite invalide : refus, pas recherche de remplacement ;
- sinon découverte existante via vswhere, critères 14.44/x64/SDK conservés ;
- même installation utilisée pour les paramètres Conan host/build.

L’exporteur doit être un programme fixe relu, écrit exclusivement dans un emplacement possédé, sans interpolation de commande utilisateur. Il exporte seulement l’allowlist nécessaire ; **pas de `set`, dump complet ou impression de map**. `VSINSTALLDIR`, versions et chemins nécessaires sont des données, non une preuve compile/link.

Le fichier de transport d’environnement et l’exporteur peuvent être temporaires : suppression des seuls fichiers possédés après consommation, avec erreur de nettoyage visible. Ils ne doivent pas devenir un nouvel environnement persistant réutilisé par build.

### 4. `toolchain.json` : utile, mais étroit

**Recommandé**, faute d’une autre preuve existante capturant entièrement les choix prepare. Le cache CMake seul ne décrit pas le choix VS/SDK ni tous les outils employés.

Champs privés minimaux :

- version privée de format ;
- configuration et arbre concernés ;
- installation VS, version/architecture MSVC, version SDK ;
- chemins absolus des outils effectivement sélectionnés et réutilisés ;
- empreintes des binaires concernés **si l’on veut affirmer leur identité**, sinon qualification explicitement limitée aux chemins/métadonnées.

Ne pas stocker la map d’environnement, recopier Request/receipt ni créer un registre général. La liaison run/plan provient du bundle et du manifeste existants.

Écriture exclusive après préparation réussie, fermeture puis collecte/empreinte **avant émission du PreparationReceipt favorable**. Build exige ce fichier admis, vérifie sa liaison et compare les choix observés ; aucune réécriture réparatrice.

Avant outils/environnement de build : contrôles bornés du cache, toolchain, `build.ninja`, configuration/arbre et mode testing attendu. Pas de parser général CMake. Le build vérifie ensuite les outils sélectionnés avant lancement ; il n’installe ni ne configure.

**Distinction impérative :** hash du JSON ou du cache ≠ hash de l’outil. Même un hash d’exécutable ne prouve pas toutes ses dépendances, son bon fonctionnement ou sa stabilité jusqu’au lancement.

### 5. Collecteur privé core nécessaire

Sous lease, autour de la commande helper, collecter **également après nonzero, interruption ou exception**, sans masquer la première cause.

Population fermée à convenir avant code :

- `toolchain.json` de préparation ;
- traces privées reliant chaque sous-commande à son rôle, argv/cwd, code/flags et logs ;
- paires `stdout.bin`/`stderr.bin` dans les emplacements numérotés autorisés.

Les rôles et bornes viennent de la séquence prepare/build relue, pas d’une découverte récursive acceptant n’importe quel fichier. Les étapes bootstrap optionnelles ont des emplacements autorisés explicites.

Exiger confinement, fichiers réguliers, absence de liens/reparse/alias vers d’autres artefacts et no-clobber. Enregistrer les fichiers disponibles même incomplets ; leur présence ne transforme pas l’échec en succès. Refuser les fichiers inattendus comme anomalie de protocole.

La capture commune reste celle du **primaire helper** ; les bruts privés décrivent ses sous-commandes, sans les présenter comme deux exécutions du même workload. Aucun helper-run ajouté.

### 6. Alternatives

- **Tout transmettre par stdout :** éviterait une partie du collecteur, mais introduirait multiplexage/parsing et fragiliserait les bruts ; non recommandé.
- **Cache uniquement :** plus simple pour le chemin CMake/CTest, insuffisant pour affirmer la continuité VS/SDK/outils prepare→build.
- **Métadonnées sans hashes outils :** possible si la continuité revendiquée reste strictement celle des chemins et versions déclarées. Ne pas annoncer « outils identiques » au sens binaire.

### 7. TDD et parallélisme minimal

Avant GREEN, fixer noms privés, champs, rôles et règles d’échec, puis deux branches réellement indépendantes :

1. **Core collecteur**, propriétaire unique `evidence.py` : fake runner produisant fichiers synthétiques ; collecte nominale et sur échec, sorties partielles, absent/inattendu, collision, lien/alias, première cause et receipt défavorable.
2. **PowerShell**, propriétaire helper/tests Windows : contexte absent/non réentrant, parent inchangé, admission avant effets, VS invalide sans fallback, export allowlist sans secret canari, Conan→CMake, build sans réparation, témoin absent/modifié/incohérent, budget et arrêt à première erreur.

Intégrer les deux producteurs avant le test de couture synthétique : témoin préparé effectivement empreinté dans `preparation.artifacts`, puis réadmis par build. Pas de nouveau test natif ni de repin implicite.

**Statut : proposition locale à ratifier, non implémentée ici.** Aucun nouveau champ public requis. L’extension d’ownership core est réelle et doit être enregistrée ; elle n’exige pas d’inventer une nouvelle autorisation utilisateur. Qualification native, GPU et performances restent **NON_EXECUTE** ; statuts ADR inchangés.