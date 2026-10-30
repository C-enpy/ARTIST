# pool-36 (@correctness-reviewer teammate)

Session: ses_e7386a3efffeP63aYOxovGrW4K
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T11:08:42.326Z — msg_18c7f67d1001rfAFEZ2mS3NcUi

## Avis : corrections requises avant intégration

Candidat annoncé **1d0fba7**, worktree pool-31. Trois défauts établis par lecture ; **aucune reproduction exécutée**, aucun edit, test ou processus lancé.

Les références ci-dessous concernent `scripts/build-windows.ps1` du candidat.

### 1. P1 — Installation MSVC correcte refusée à cause du séparateur final
**Lignes1045–1046 — `Read-VerificationMsvcEnvironmentLeaf`**

- **Déclencheur :** installation sélectionnée `C:\VS`, export `VSINSTALLDIR=C:\VS\`.
- **Attendu :** reconnaître la même installation et employer son chemin normalisé pour Conan host/build.
- **Observé :** `GetFullPath` conserve le séparateur final ; la comparaison textuelle échoue avant la normalisation ligne1052.
- **Impact :** préparation nominale bloquée, notamment par découverte `vswhere`. Ce n’est pas seulement hypothétique : le fichier installé `Common7/Tools/vsdevcmd/core/vsdevcmd_start.bat:84` construit explicitement `VSINSTALLDIR=%CD%\`.
- **Vérification minimale :** conserver l’installation synthétique sélectionnée sans séparateur et ajouter seulement `\` au `VSINSTALLDIR` exporté. Attendre un succès et des options Conan host/build cohérentes. La donnée actuelle de `test_native_binding.py:205` masque cette différence.

### 2. P2 — Le build refuse le cache produit par son propre prepare pour les cibles de tests
**Lignes1123,1134 — `Assert-VerificationPreparedTree` ; ligne1242 — dispatch prepare**

- **Déclencheur :** préparation avec `ArtistLib_tests` ou `ArtistLib_component_tests`.
- **Attendu :** réadmettre le cache valide créé par la commande de configuration.
- **Observé :** prepare passe `-DCMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE=PRE_TEST` **sans type**. Cette variable, non déclarée en cache typé par le projet ou le module GoogleTest consulté, demeure `UNINITIALIZED`. Le validateur exige pourtant exactement `:STRING` et refuse l’autre type.
- **Impact :** prepare peut réussir, puis build refuse avant lancement sur un arbre conforme à sa commande productrice.
- **Vérification minimale :** couvrir synthétiquement la cohérence producteur/consommateur : soit produire explicitement `:STRING` et vérifier cet argv, soit admettre précisément la représentation effectivement produite. Conserver les refus des valeurs incorrectes et doublons ; aucun configure réel nécessaire.

### 3. P2 — Une variable d’environnement `Keys` corrompt l’observation réelle du parent
**Ligne660 — `Read-VerificationHostObservationLeaf`**

- **Déclencheur :** environnement Windows contenant une variable légale `Keys=canary`.
- **Attendu :** copier toutes les entrées du dictionnaire, puis appliquer l’allowlist.
- **Observé :** `$block.Keys` est un accès PowerShell shadowable sur la Hashtable retournée par `GetEnvironmentVariables`. La valeur de l’entrée `Keys` remplace la collection des clés. Le snapshot perd notamment `PATH` et `SystemRoot`.
- **Impact :** refus de tooling/MSVC malgré un environnement autrement valide. Les doubles remplaçant cette feuille ne couvrent pas le défaut ; les wrappers utilisent déjà `PSBase.Keys` pour éviter ce piège.
- **Vérification minimale :** exercer la boucle réelle avec une `IDictionary` synthétique contenant `PATH`, `SystemRoot` et `Keys`. Vérifier la copie complète puis l’exclusion du canari, sans mutation OS.

## Question distincte — encodage du transport cmd

**Lignes1028–1029 — `Read-VerificationMsvcEnvironmentLeaf`**

Le batch utilise `Encoding.Default.CodePage`, sans établir la page utilisée par `cmd.exe`. Pour un chemin non ASCII, leur correspondance reste à établir. **Je ne classe pas cela comme défaut natif reproduit.** Une vérification synthétique des octets peut préparer la qualification, sans autoriser ici l’exécution de cmd/vcvars.

## Portée et limites

- Contrats, plan QA§13/14, rapports122–133, binding, entrée/dispatch et consommateurs concernés examinés.
- Serena reste attaché au canonique avec serveur C++ ; analyse PowerShell du worktree effectuée par lectures ciblées.
- Comptages et identités du GREEN repris des preuves lead, non réexécutés ni recalculés.
- Couture core/helper, ancien oracle de fermeture, qualification native et gap Dispose/writers restent les limites annoncées, pas de nouveaux findings.
- Aucun changement structurel ni revendication de performance proposé. Ces constats ne constituent pas une preuve exhaustive.

## assistant — 2026-10-30T11:34:50.796Z — msg_18c9756a7001hR66AvwXifERPw

## Réaudit — candidat `9680f2d`

**F2 et F3 peuvent être fermés. F1 conserve un résidu bloquant P2.** Lecture seule ; aucun edit, test ou processus lancé.

### Résidu F1 — même installation explicite refusée en build
**`scripts/build-windows.ps1:976` — `Read-VerificationMsvcEnvironmentLeaf`**

- **Scénario :** prepare reçoit `-VisualStudioPath 'C:\VS\'`. La correction normalise désormais cette sélection en `C:\VS` et publie cette forme dans le témoin. Build reçoit ensuite **le même argument explicite** `C:\VS\`.
- **Attendu :** reconnaître la même installation, puis continuer sans découverte ni réparation.
- **Observé par lecture :** le garde build ligne976 compare encore les deux `Get-VerificationAbsolutePath` avant la normalisation ajoutée lignes997–1001. Le séparateur final conservé provoque `InvalidMsvcEnvironment`.
- **Impact :** prepare→build échoue malgré une sélection explicite inchangée. Le chemin habituel de l’adaptateur, sans cet argument, n’est pas concerné.
- **Vérification minimale :** cas synthétique prepare puis build avec sélection explicite terminée par `\`, témoin normalisé sans séparateur, et assertions conservées contre une installation réellement différente. Aucun outil natif nécessaire.
- **Correction locale attendue :** appliquer la même normalisation d’identité au garde build, sans modifier globalement `Get-VerificationAbsolutePath`.

Le nouveau test F1 (`test_review_regressions.py:158–170`) couvre le suffixe **exporté** et les options Conan host/build, pas cette répétition du paramètre en **phase build**.

### Corrections confirmées

- **F1 initial :** comparaison sélection/export corrigée lignes997–1063 ; préservation explicite des racines.
- **F2 :** producteur désormais `…DISCOVERY_MODE:STRING=PRE_TEST` ligne1253 ; consommateur strict et négatifs conservés. Mapping de l’ancien littéral approprié.
- **F3 :** vraie boucle utilisant `$block.PSBase.Keys` ligne660 ; sonde bornée aux statements source, sans observation OS réelle.
- **Garde factice VSINSTALLDIR :** adaptation nominative, valeur sélectionnée synthétique confinée et comparaison exacte ; aucun assouplissement général constaté.

Le journal GREEN existant confirme les trois régressions réussies. Les autres comptages et hashes restent attribués au lead, sans réexécution.

**Avis : ne pas fermer complètement F1 ni accepter ce delta avant traitement du résidu ci-dessus.** Encodage non ASCII, qualification native, couture après intégration et gap Dispose/writers restent les limites annoncées ; aucun nouveau constat sur ces sujets. Aucun audit global ni preuve exhaustive.

## assistant — 2026-10-30T11:47:45.749Z — msg_18ca329d00011pF6mJu0DwiP9S

## Avis favorable borné — candidat `dc240fc`

**F1B fermé ; F1–F3 désormais fermés dans le périmètre revu. Aucun nouveau défaut démontré.**

- **Localisation :** `scripts/build-windows.ps1:976–994`, `Read-VerificationMsvcEnvironmentLeaf`.
- **Correction :** requested/prepared sont validés comme chemins absolus puis normalisés symétriquement **avant comparaison**, avec préservation des racines. Le témoin reste ensuite la source de sélection ; aucun fallback ajouté.
- **Tests relus :** suffixe explicite conservé dans les données, témoin inchangé, succès attendu pour la même installation ; répertoire différent toujours refusé pour la cause exacte, sans capture ni processus factice. L’option de fixture demeure neutre par défaut.
- **Preuves existantes :** journaux GREEN consultés : trois contrôles F1B et une régression réussis. RED et autres non-régressions attribués au lead, sans réexécution par moi.

**Réception du helper recommandée pour intégration**, sans assimiler ces cas synthétiques indépendants à une couture prepare→build sur un même bundle.

Restent distincts : couture main/collecteur après intégration, mapping des anciens tests avant QA finale, encodage non ASCII non établi et gap Dispose/writers hérité. Aucune qualification native, moteur ou GPU démontrée.

Lecture seule : aucun edit, processus, test ou sous-agent lancé. Identités/propreté reprises des preuves lead, non recalculées. Avis non exhaustif.