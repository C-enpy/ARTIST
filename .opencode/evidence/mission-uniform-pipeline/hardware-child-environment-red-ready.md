# Environnement enfant matériel — TESTS_RED_READY

Auteur pool-26, tâche `task_mvumhg9g_005j_81env1`, 2026-10-30.
**Candidate pour revue lead, pas intégration ni RED observé.**

Révision après relecture lead du candidat `0a60636482b68e99132bd2819c7d8df27864d4ef` :
correction de confidentialité des logs **TESTS uniquement**, avant tout RED.
La fixture parent est maintenant entièrement synthétique ; aucune valeur hôte
n'est copiée dans l'environnement enfant ou le dictionnaire attendu affichable
par unittest. `patch.dict(clear=True)` restaure le parent extérieur à la sortie,
sans afficher ni comparer ses valeurs réelles. Les dix méthodes et assertions
H1–H6 restent inchangées. Ce point ne constitue ni finding production ni RED.

## Entrée et périmètre

- Worktree vérifié avant écriture :
  `C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-26`.
- Branche : `ensemble-56a896a3-6298-423c-bfa5-014fa7983cb2` ; HEAD entrée exact
  `b89110594731737f5f813c4017599a195e56033d`, état initial propre.
- QA §12 déjà commité dans ce HEAD avant admission ; contrat privé commité
  `7a5ed1632278add245a6d65b7f86875b60907b36`. Contrat général et rapports100/101/103 lus.
- Ajouts uniquement : cette note et
  `scripts/tests/verification_core/test_hardware_child_environment_contract.py`.
  Aucun ancien test/assertion, code production, adaptateur, CMake ou workflow modifié.
- CMake, presets, Conan2/CMakeDeps/CMakeToolchain et CI inspectés seulement.
  Aucune commande native de configuration/build/découverte/sonde autorisée ici.

## Convention de refus ratifiée avant RED

Convention ratifiée par le lead pour le futur GREEN minimal après RED causal :
le runner refuse la description hardware invalide par **ValueError mentionnant
la clé fautive**, avant factory. Aucun nouveau type/code public et aucun code
enfant artificiel. Les tests ne capturent pas une exception quelconque :
`assertRaisesRegex(ValueError, clé)` puis zéro appel factory.
Le test de phase utilise le vrai `evidence.prepare` et son traitement existant :
FAILED/code4, cause attribuable, jamais précondition/code3 ni PASS. Aucun changement
du collecteur n'est demandé. Une erreur de capture n'est pas ce refus de transport.

## Scénarios et confinement proposés

Dix méthodes, HT1–HT6 de QA §12 :

- HT1 : trois parents (chaque forçage puis paire) × workload/CTest/découverte ;
  comparaison du dictionnaire enfant entier, six valeurs exactes, absence réelle
  des deux clés de forçage. Cas supplémentaire de valeurs non vides mais non
  validées sémantiquement : le runner ne devient pas un second validateur core.
- HT2 : cinq champs hors sélecteur × manquant/vide, parent complet ne réparant rien.
- HT3 : deux clés de forçage × `1`/`0`/vide dans delta, refus avant factory.
- HT4 : hardware seulement ambiant, delta sans sélecteur, sélecteur vide/autre,
  CPU et logiciel explicite ; héritage et nettoyage historique inchangés.
- HT5 : mêmes instances de runner dans les ordres CPU→hardware→logiciel et inverse,
  avec refus HT2/HT3 intercalés avant le profil suivant ; parent complet comparé
  avant/après chaque contexte, y compris les refus.
- HT6 : codes bruts 0/10/20/30, octets invalides UTF-8/CRLF, timeout/interruption,
  causes de création/lecture/nettoyage, statut pipeline d'un refus de transport.

Réemploi de `PUBLIC_CONTEXT` du test de transport et de `_FileCaptureRig` du test
de capture (fichiers privés, waits finis, offsets indépendants). Factory injectée
strictement en mémoire : argv/cwd/shell/env vérifiés, compteurs et rejet d'un
second lancement. Aucun enfant réel, même factice ; aucun outil argv exécuté.
Sentinelles subprocess/OS, réseau et threads actives avant imports locaux dans
setUp. Imports sans appel de main/producteur/GL. Captures et cwd dans les seules
TemporaryDirectory possédées, fermeture et nettoyage dans finally. Les fichiers
de la fixture de phase sont eux aussi synthétiques et possédés. Les dictionnaires
parent/child/expected comparés ne contiennent que des valeurs synthétiques possédées.

## Vérification réellement exercée

- **Windows, analyse statique AST seulement : PASS syntaxique**, dix méthodes
  `test_*` comptées. Interpréteur du sandbox Context Mode observé :
  `C:/Python313/python.exe`, Python3.13.1. Fichier cible lu par chemin absolu ;
  cwd réel retourné par ce sandbox : `C:/Users/djden/source/repos/ARTIST`, distinct
  du worktree source vérifié. Aucun import du module cible.
- SHA256 du nouveau test :
  `8caaeaa342177d5d2711c153d051af59913cfe186e558ca22c6b5bf34bdd9383`.
- Windows tests dynamiques : **NON_EXECUTE**, 0 exécuté, découverte unittest non
  lancée ; Linux : **NON_EXECUTE**. RED/GREEN/non-régression : **INCONCLUSIVE**.
- Aucun build, suite moteur, CTest réel, contexte GL, rendu, mesure, installation,
  cloud ou sous-agent. Aucune base compile_commands ni configuration clangd modifiée.

Analyse AST exacte transmise à Context Mode (`language=python`) :

```python
import ast, pathlib, hashlib, sys
p = pathlib.Path('C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-26/scripts/tests/verification_core/test_hardware_child_environment_contract.py')
b = p.read_bytes()
t = ast.parse(b, filename=str(p))
methods = [n.name for n in ast.walk(t) if isinstance(n, ast.FunctionDef) and n.name.startswith('test_')]
print('AST_ONLY_NO_TARGET_IMPORT: PASS')
print('test_methods:', len(methods))
print('sha256:', hashlib.sha256(b).hexdigest())
print('interpreter:', sys.executable)
print('version:', sys.version.split()[0])
print('source:', p)
print('sandbox_cwd:', pathlib.Path.cwd())
```

## Commande RED à exécuter par le lead après revue, non exécutée par l'auteur

Cwd : le worktree ci-dessus (ou intégration contenant exactement ce test et la
production d'entrée, en conservant ses identités). PowerShell :

```powershell
& 'C:/Python313/python.exe' -B -m unittest discover -s scripts/tests/verification_core -p 'test_hardware_child_environment_contract.py' -v
```

Attendu : dix méthodes découvertes, aucun skip ; échecs d'assertion causaux HT1
(forçages encore présents) et HT2/HT3 (ValueError attendu absent), pas une panne
d'import ni une sentinelle native déclenchée. HT5 et le contrôle de phase doivent
également révéler le contrat manquant. Aucun comptage de failures inventé : les
subtests sont regroupés et seul le lead peut constater les nombres réels.

Première cause **statique** : `SubprocessRunner.run`, lignes44–47 à l'entrée,
retire les six contextes puis applique le delta mais conserve les deux forçages
logiciels parent ; il n'admet/refuse pas le transport hardware. Aucun correctif
production dans cette remise. GREEN seulement après RED causal et GO lead.

Non-régression ciblée proposée au lead après GREEN, mêmes cwd/interpréteur :

```powershell
& 'C:/Python313/python.exe' -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_transport_contract.py' -v
& 'C:/Python313/python.exe' -B -m unittest discover -s scripts/tests/verification_core -p 'test_file_capture_contract.py' -v
```

## Localisation / impact / limites

Localisation : environnement effectif passé à `popen_factory`, sans dépendance à
argv. Impact futur borné à la construction d'environnement de runner.py ; les
captures et la gestion primaire restent sous les oracles existants. Serena
canonique ARTIST confirmé ; serveur C++ seul, extraction Python refusée comme
non couverte, donc lecture ciblée des fichiers de ce worktree, aucun index
canonique utilisé pour prétendre couvrir ce diff. Pas de revue indépendante ni
preuve Linux nouvelle. Revue fresh après GREEN et intégration restent au lead.
ADR0001/2/4/6/7 Proposed,0003/5 Accepted/partial,0008 baseline Windows Debug : inchangés.
