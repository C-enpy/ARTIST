# Windows — déclaration hardware par invocation : TESTS_RED_READY

## Autorité, HEAD et scope

Auteur pool-23, tâche `task_mvukzhsc_0052_f1bbfq`, worktree propre à l'entrée :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-23`,
branche `ensemble-4b686588-c97c-46ab-99a1-e176a14b9184`, HEAD initial
`84b9f80ca276665fdfb7e19126a82f664ca2c4bc`. Pas d'intégration revendiquée.

Lectures canoniques autorisées, sans copie de branche :
`hardware-child-environment-v1.md` (contrat ratifié commit lead
`7a5ed1632278add245a6d65b7f86875b60907b36`) et qa-plan.md §12 HT1/H1/H3.
Le runner commun corrige l'environnement effectif SEULEMENT lorsque le delta
porte une déclaration hardware explicite complète. Il ne doit pas interpréter
argv ou le nom d'un GPU pour inventer la déclaration.

Preuve réutilisée, exécutant lead : 47/47 Windows au candidat84b9f80,
194 core dont191 satisfaits+3symlinkSKIP,28/28 contexte,31/31 scénarios, sans
failure/error et empreintes stables. Logs
`out/verification-synthetic/windows-python-binding-green-2026-10-30T07-11-03-266Z/`.
Ce GREEN est historique, pas un rerun auteur, et ne couvre pas le raccord manquant.

Deux ajouts seulement : nouveau `test_hardware_binding_transport.py` et cette
note. Aucun changement de production,47 anciens tests/fixtures, PowerShell,
garde fermée, WRAPPERS/NATIVE/DISPATCH/legacy ou pins. Core/runner/modèles/verify/
C++/CMake/presets/workflows exclus. Pas de nouveau schéma ou stratégie GPU.

## Gap causal et tranche proposée

Source84b9f80 : les workloads hardware décrivent env_delta vide, comme CTest
discovery. Seule l'exécution CTest déclare les six champs. Un runner qui déclenche
l'assainissement uniquement sur déclaration explicite ne pourra donc pas assainir
les deux premières catégories. Aucun faux PASS GPU reproduit ou prétendu ici.

Les sept nouvelles méthodes testent les descriptions du VRAI WindowsAdapter,
avec vrais plans/receipts admis et fichiers synthétiques produits par le seed
en processus déjà utilisé dans les47 tests. Le seed prépare uniquement les
receipts ; aucune factory de faux processus, fake Popen ou SubprocessRunner.run
n'est ajouté/utilisé. Le nombre d'appels du seed runner est figé avant chaque
description et doit rester inchangé. Sentinelles de processus/réseau/écriture
existantes réutilisées via helpers, sans héritage d'un TestCase ni redécouverte
des27 anciens tests.

Le helper hardware commence par une égalité de l'ensemble des clés : déclaration
absente = **assertion explicite**, avant accès par clé. Aucun KeyError/import/
scaffold ou outil absent accepté comme cause RED. Il exige les cinq valeurs
plan/reçu exactes, un request_id explicite non vide, et pour les workloads
request_id env égal à argv. Les six champs forment tout le delta ; aucune
réintroduction de LIBGL_ALWAYS_SOFTWARE/GALLIUM_DRIVER, même vide/None.

## Oracles et rattachement HT1

1. Hardware workload : chaque route réelle pass-name/prepared-float a les six
   valeurs exactes, dont l'identité visible en argv. Sous-cas par route.
2. Hardware découverte CTest : même transport complet avant le run ; pas de
   déduction depuis le futur argv ou une variable parent.
3. Hardware exécution CTest : contrôle de non-régression de la déclaration déjà
   présente, exactitude et absence de variables de forçage supplémentaires.
4. Mixte : workloads+discovery+run, descriptions répétées identiques ; IDs
   distincts pour TOUTES les invocations, pas un ID partagé discovery/run.
   Sous-cas par catégorie, assertion finale sur la population déclarée.
5. Deux bundles du même plan : descriptions répétables au sein d'un run, tous
   les IDs différents entre runs. Une absence de déclaration arrête ce test
   avant les oracles aval ; pas de couverture anticipée annoncée au RED.
6. Parent synthétique hardware+forçage logiciel : descriptors inchangés et
   transport explicite exact, parent inchangé. Lecture/sanitization du parent
   ne peut pas remplacer une déclaration ; sous-cas par invocation.
7. CPU : aucun contexte ni forçage ajouté au delta malgré le parent synthétique.
   Ce test ne réclame PAS que le runner retire l'héritage logiciel en mode CPU.

Les tests01/02/04/05/06 doivent mettre en évidence les déclarations manquantes
au code actuel ;03/07 sont contrôles déjà satisfaisables. Les sous-cas ne sont
pas annoncés comme un comptage exécuté. Aucun test ne prétend inspecter
l'environnement EFFECTIF reçu par un enfant : cette preuve appartient au runner
commun et à sa consommation après intégration réelle de la correction pool26.

## Commande RED du lead (NON exécutée par l'auteur)

Depuis le worktree auteur :

```powershell
Set-Location scripts/tests/verification_windows
C:/Python313/python.exe -B -m unittest -v test_hardware_binding_transport
```

Sept méthodes proposées. Attendu : nonzero, assertions
`missing/extra explicit six-field hardware transport in env_delta` sur workloads
hardware et découverte CTest ; zéro erreur/skip, pas de cause d'import/scaffold
ou d'exécution native. Le lead conserve nouvelle sortie exclusive sous
`out/verification-synthetic/`, empreintes/commande/cwd/interpréteur/comptages et
bruts. Aucun nouveau test des47 ni suite moteur à lancer par l'auteur.

Après RED observé seulement, un futur correctif autorisé pourra réutiliser un
builder privé de six champs par invocation, avec IDs discovery/run distincts et
identité workload env==argv. Pas de duplication de l'assainissement runner, de
lecture os.environ en production, de forçage vide ou d'API/schéma supplémentaire.
Cette remise ne code PAS ce GREEN.

## Matrice réelle et limites

- Lecture des contrats/Git et AST du nouveau module réellement accepté via
  Context Mode sous `C:/Python313/python.exe`3.13.1, sept méthodes comptées dans
  l'AST, sans import projet. Aucun unittest, programme factice, PowerShell ou
  outil natif exécuté par l'auteur.
- Nouveau RED/GREEN et environnement effectif : NON_EXECUTE par l'auteur.
  GREEN47 historique attribué au lead, pas preuve de ce nouveau contrat.
- Windows/Linux natifs, toolchain/STL/ABI, CPU/GL moteur, rendu, GPU, benchmark,
  installation et cloud : NON_EXECUTE. Pas de performance ni qualification GPU.
- Le helper fonctionnel reste fermé ; sa future ouverture est indépendante de
  cette déclaration et du runner. Pas de jonction06/discovery native globale.
- Aucun sous-agent/team tool/auto-claim/clôture, politiques et scores inchangés.

Remise TESTS_RED_READY au lead, puis STOP avant production GREEN.
