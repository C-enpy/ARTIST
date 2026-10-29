# 25 — F8 : erreur OS masquée en EOF, frontière corrective ouverte

2026-10-24 — candidat HEAD `ecb548b64da8dd932f9e201edf21d6f2005313e8`. Auteur de la proposition : pool-9 ; persistance, compilation et exécution : lead. H_Q64d97dc et H_A354b53f précèdent la sonde. Ce rapport recoupe les artefacts existants ; aucun nouveau run.

## Résultat exécuté et recoupé

Sonde Windows autonome, sans nouvelle seam/API moteur, installation, service ou dépendance. Fixture acquise exclusivement par CreateDirectory/CREATE_NEW ; fichier4608 octets déterministes. Séquence réelle :

- Sans verrou : ReadFile, ifstream et Reader retrouvent exactement4608 octets ; publication Reader `0,4608,`.
- Sous LockFileEx : ouverture de contrôle réussie, ReadFile échoue `ERROR_LOCK_VIOLATION=33`, zéro octet. ifstream reste ouvert, masque badbit=4 inchangé ; **aucune exception**, `bad=0,eof=1,fail=1,gcount=0`, résultat vide. Reader retourne normalement et publie vide : événements `0,0,`.
- Après déverrouillage : les trois voies retrouvent exactement4608 octets et Reader publie `0,4608,`.
- UnlockFileEx, CloseHandle, DeleteFile et RemoveDirectory réussissent ; fixture absente après processus selon contrôle runner.

Processus **exit10**, marqueur `CAUSAL_CANDIDATE_EOF_PUBLICATION_NOT_CATCH_PROOF`. Le verrou explique causalement la différence baseline/échec/recovery. Ce n'est pas une preuve du catch ifstream::failure, ni de close échoué.

## Commandes et identité

Compilation réellement effectuée via `out/tooling/build/Scripts/python.exe out/tooling/run-build.py b3-f8-lock-probe-build <cl et options dérivées> <source>`. Commande intégrale conservée dans le log build. Options de l'entrée TraceableExceptionTests.cpp de compile_commands.json, découpées par CommandLineToArgvW ; seuls source/sorties et mode compile-only remplacés pour le lien autonome. MSVC14.44 Debug, /RTC1, MDd, Zi, C++latest et includes réels conservés. `__mock_gl__` conservé, aucun matching dans ARTIST/include ; aucune fonction GL appelée.

Exécution : `out/tooling/build/Scripts/python.exe out/tooling/run-build.py b3-f8-lock-probe-run <sortie>/f8-lock-probe.exe <sortie>/owned-input`, avec `<sortie>` = chemin absolu canonique `out/qa-f8/calibration-20261024-01`.

Source, exécutable, objets/PDB et logs conservés dans cette sortie canonique ; copies des logs et source archivées sans écrasement lors de cette reprise :
- `f8-lock-probe.cpp` SHA256 `9a067758f87c105aa0bb1c724a7f5faaa54f77db30d018839e13bcf8abf0ecae` ;
- `f8-lock-probe.exe` SHA256 `d55e0c93e3d859b6b85edbb2127c06ba3696c5779732ae981c125efbba8f550d` ;
- `run.log` SHA256 `d530a7f2bbbad9922e5121a62bf4c4376a90d9fc6b7dbe5df013e78e7a891962` ;
- `build.log` SHA256 `115823036e717e4f8627c3d9ac449e57df9d666f56f6b3691ca12e8bf63ea7c1`.

Reader SHA avant/après run `ce97d9e3ecfcf163ab757bb9c1dbf29116fb52505443c17a8725da5c0a90a284`, inchangé. Aucun correctif Reader autorisé ou appliqué.

## Qualification et décision à préparer

**Observé :** échec OS réel et publication vide par Reader sous cette STL Windows. **Non prouvé :** exception de lecture réelle traduite, détection portable de cette erreur via les seuls flags istream, fermeture réellement échouée.

Le H_Q prévoyait explicitement ce cas comme candidat défaut à retourner au lead avant correction. Pool-9 reçoit les résultats pour analyse en lecture seule ; consultation architecte requise sur la frontière. Un simple contrôle de taille ne sera pas adopté sans contrat de mutation/TOCTOU. Aucun masque, setstate, rdbuf ou handle caché modifié pour fabriquer la preuve.

Options à examiner : conserver un diagnostic partiel honnête sous le contrat actuel, ou proposer un dispositif correctif borné nécessitant décision explicite si la frontière/API change. Recommandation immédiate : figer les preuves et analyser, pas changer Reader à l'aveugle. Aucune décision supplémentaire demandée à l'opérateur avant cette analyse.

Consultation publiée : `task_mvmk6jba_0014_bp09tc`, affinité engine-architect, analyse PM/LLM58/R60, mandat lecture seule. Un premier spawn sans tâche a été refusé (« require claim_task »), sans worker créé. Après publication, le spawn configuré fresh/worktree:false a été refusé parce que la tâche était déjà in_progress : l'auto-dispatch l'avait prise. Aucun doublon créé ; profil/contexte effectifs et restitution restent à confirmer par le retour natif avant d'accepter l'analyse. Pas de nouveau spawn concurrent ni de modèle forcé. Le retour B4 textuel a également été demandé au même auteur, indépendamment de l'analyse F8.

Retour pool-13 : rapport25 absent de son worktree (File not found), QMD non exposé ; Reader et ADR/principes lus. Cause documentaire identifiée : ce rapport est encore non commité et n'a pas été transmis au worktree. Le lead a fourni directement dans le brief les clauses H_Q17/H_A08, les résultats baseline/verrou/recovery, hashes et limites, sans nouvelle tentative de chemin externe ni élargissement de permission. Analyse finale attendue ; aucun défaut runtime déduit de ce blocage documentaire. Pool-9 confirme en analyse que les flags seuls ne distinguent pas l'erreur d'un EOF légitime, qu'un size-check ne règle pas mutation/ABA, et propose des alternatives non encore autorisées. Patch minimal B4 textuel reçu, non appliqué ni validé à ce stade.

## État du lot

Catch réel **INCONCLUSIVE/non démontré** ; close réel **DEFER/non couvert**, aucune clôture globale F8. B6 intégré (rapport23), B1 trois ordres477/477 mais histoire non fermée (rapport24), B4 caractérisation10/10 et refactor encore attendu. ASan final et revue finale non exécutés. Les deltas de profils/GrepAI/harness concurrents restent hors ownership et préservés. ADR0003/0005 Accepted/partial,0008 Windows Debug seulement ; aucun statut promu.
