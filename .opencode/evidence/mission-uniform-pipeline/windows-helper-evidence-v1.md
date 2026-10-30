# Preuves privées du helper Windows — contrat local v1

Ratification lead après `result_mvupwdk8_0001_thq007`, proposition auteur
`result_mvups06p_0000_as9dx4`. Complète le binding Windows sans champ public,
Request/receipt/CommandSpec nouveau, registre d'outils ou backend de capture.
Le GREEN attend les tests et le complément QA commité.

## Responsabilités

Le helper possède son contexte privé non réentrant par invocation : root dérivé
du vrai PSScriptRoot, phase/configuration/bundle/arbre admis, map destinée aux
enfants, outils absolus sélectionnés, logs et échéance globale. Création après
admission, absence refusée par les feuilles, retrait finally. Aucun contexte
Global, cache entre invocations, fallback legacy ou mutation de l'environnement OS.
Read/Write-EnvironmentLeaf opèrent seulement sur cette map ; null retire, vide
reste présent. Les wrappers et primitives natives pinés ne sont pas modifiés.

Chaque commande possède un Process unique ; binding/factory propriétaire du
Dispose, primitive propriétaire de ses streams comme actuellement. Le budget
restant borne chaque lancement, pas un timeout complet répété. Code absent,
timeout, exception et échec de nettoyage ne deviennent jamais succès. La première
cause et les informations disponibles sont conservées. Le gap Dispose/writers
hérités reste ouvert, pas de garantie nouvelle sur les descendants.

## Population fermée des journaux

Sous `logs/windows-helper-prepare/`, rôles autorisés, chacun au plus une fois :
`python-venv`, `python-ensurepip`, `python-pip`, `vswhere`, `msvc-env`,
`conan-install`, `cmake-configure`. Les premiers rôles sont optionnels selon
tooling/VS préparés ; les trois derniers sont nécessaires à un prepare réussi.
Sous `logs/windows-helper-build/` : `msvc-env`, `cmake-build`, nécessaires au
succès. Pas d'autre rôle, pas de CTest/workload ou helper-run.

Chaque rôle est un sous-répertoire à nom exact ; seuls `stdout.bin`, `stderr.bin`
et `command.json` y sont autorisés. La trace privée contient exactement :
`format` = `artist-helper-command-v1`, `role`, `program`, `command_line`, `cwd`,
`returncode` (entier ou null), `timed_out` et `interrupted` booléens, `error`
(texte ou null). `command_line` est la chaîne effectivement fournie au StartInfo,
pas un argv reconstruit approximativement. Outils/cwd absolus, pas de shell libre.
Écriture exclusive ; trace fermée avant retour ou avec l'issue disponible en erreur.
Les paires brutes peuvent être partielles en échec ; pas de troncature ajoutée.

Fichiers de transport temporaires autorisés dans `_transport/` de chaque phase :
`export-msvc.ps1`, `load-msvc.cmd`, `msvc-env.json`. Programmes fixes relus,
emplacements exclusifs et possédés, allowlist seule ; aucun `set` global, dump
d'environnement ou interpolation de commande utilisateur. Suppression des seuls
fichiers possédés après consommation, cleanup failure visible. S'ils subsistent
en échec, le collecteur enregistre ces seuls fichiers disponibles sans les réutiliser
comme environnement de build. Aucun répertoire arbitraire découvert récursivement.

## Témoin de préparation étroit

`logs/windows-helper-prepare/toolchain.json`, écrit exclusivement et fermé
**après préparation réussie**, avant retour du helper. Format privé plat avec
exactement douze valeurs texte non vides :
`format` = `artist-helper-toolchain-v1`, `configuration`, `build_directory`,
`visual_studio_installation`, `msvc_version`, `msvc_architecture`,
`windows_sdk_version`, `python`, `conan`, `ninja`, `cmake`, `cl`.

Choix minimal ratifié : **chemins et métadonnées seulement**, pas de hashes
de binaires outils dans ce témoin. Son empreinte admet ces octets déclaratifs,
pas l'identité binaire des outils, leurs dépendances, leur bon fonctionnement
ou leur stabilité jusqu'au lancement. Ne jamais annoncer « outils identiques »
au sens binaire ; seules cohérence des chemins/installations et versions déclarées
sont contrôlées. Les identités des workloads produits restent celles du core.

Prepare : VS explicite invalide refuse sans remplacement ; sinon découverte
existante strictement 14.44/x64/SDK. VSINSTALLDIR observé fournit les deux options
Conan host/build. CMake préinstallé seulement ; Conan/Ninja/venv réutilisent les
requirements épinglés existants après opt-in. Aucun configure/build/test dans
Initialize-Tooling ; prepare exécute ensuite Conan2 puis configure seulement.

Build : témoin présent dans PreparationReceipt.artifacts et empreinte réadmise
par le core avant enfant ; lecture des mêmes données privées par le helper,
liaison configuration/arbre vérifiée, choix observés comparés, aucune réécriture.
Avant tooling/env/process : cache/toolchain/build.ninja présents et conformes à
l'arbre/configuration/Ninja, workloads ON et mode testing attendu. Contrôles
bornés, pas parser général CMake. Aucun bootstrap/configure ou réparation build.

## Collecteur core, sous lease

Activation seulement pour le transport Windows déjà possédé/admis par le sink.
Après commande helper, **également sur nonzero, interruption et exception**,
collecter la population disponible ci-dessus avec sink.artifact, avant receipt.
Pas de collecte des révisions Git ou commandes génériques sans transport helper.
Le témoin et les rôles nécessaires sont obligatoires pour un receipt favorable.
Dans ce cas toutes les traces présentes portent un code entier0, aucun flag timeout/
interruption et aucune erreur ; leurs rôles correspondent à leurs répertoires.
La configuration du témoin vaut exactement Debug ou Release selon le plan.
Un échec garde les fichiers partiels disponibles sans inventer un témoin de succès.

Confinement et fichiers réguliers ; liens/reparse/alias/hardlinks vers d'autres
artefacts refusés, no-clobber et pas d'adoption d'un ancien dossier. Fichiers ou
répertoires inattendus : anomalie de protocole, aucune réparation ou suppression.
Ce contrôle est borné aux chemins relus, pas une sandbox filesystem hostile.
Le collecteur ne remplace ni les logs communs ni la première cause. Une anomalie
secondaire est enregistrée comme telle, pas transformée en succès ; priorité des
codes existante conservée. Les fichiers inconnus restent sur disque pour diagnostic,
sans les intégrer silencieusement à une population supposée valide.

Le core valide les clés/types privés, les doublons JSON, format/configuration/arbre
et cohérence de la population. Il ne découvre pas VS, n'exécute pas d'outil et ne
duplique pas le validateur de capacités MSVC. Tout témoin absent/modifié/étranger
refuse avant build, sans réécriture réparatrice ou fallback.

## Tests et ownership avant code

Deux producteurs indépendants après QA :
- Core : propriétaire unique des hooks Windows dans evidence.py et nouveaux tests
  synthétiques. Fake runner créant journaux/témoin, collecte succès/erreur/exception/
  interruption, fichiers partiels/inconnus/alias, première cause et receipt défavorable.
- PowerShell : propriétaire helper et nouveaux tests de feuilles ; contexte privé,
  export allowlist canari, factory mémoire, budget, cwd/commandline/env/logs/codes,
  Dispose/erreurs et continuité déclarative prepare→build. Primitives pinées intactes.

Avant changement des fakes historiques de succès Windows, le propriétaire core
propose un mapping explicite : ils doivent produire une preuve privée synthétique
conforme, jamais retirer les assertions métier pour accepter un témoin absent.
Les deux producteurs sont intégrés avant test de couture : témoin réellement
empreinté dans preparation.artifacts puis réadmis avant build. Aucun vrai outil,
helper complet, moteur, GPU, installation ou cloud exécuté pendant cette mission.
