# 20 — Accès B4/B6 confirmé, oracles bornés et préflight runner

Date : 2026-10-24. Étape : préparation des preuves TDD ; aucun test/compile moteur encore exécuté.

## Retours après rattachement

Pool-11 confirme `.git` comme fichier pointeur de son worktree et H_Q relatif lisible ; pool-10 confirme la racine native `qa-harness-b4-u`, son pointeur et H_Q lisibles. Les résultats `failed` antérieurs décrivaient une précondition, pas un échec produit ; ils sont dépassés sur l'accès par ces retours. Aucun fichier modifié par les auteurs.

L'accès local est désormais observé pour ces deux membres, pas déduit de session_move. Serena reste canonique : aucune réactivation de la connexion partagée.

## B6 — choix d'oracle du lead

L'auteur propose de transposer exactement la branche conditionnelle du header dans un TU indépendant sous MSVC, en remplaçant seulement le type `std::stacktrace` par une sonde locale itérable. Appel réel de getStackTraceString via objet const. Contrôle positif jetable : ajout const au helper dans le TU seulement ; il doit compiler avant de qualifier le rejet négatif attendu const-this/C2662 ou équivalent.

Le lead accepte cette **preuve de langage transposée**, pas une validation GCC/libstdc++ ni un remplacement de CheckCxx23. À produire : texte générateur/TU, transformations exhaustives, source pin/hash et commandes ; persistance/revue/exécution par lead. Aucun changement production avant positif valide et RED causal négatif. Si autre erreur/toolchain absente : INCONCLUSIVE. Correction future bornée au const du helper privé, non appliquée.

Recoupement lead : header relu lignes18–45 ; appel const vers format_stacktrace non const confirmé. Matching exact dans ARTIST retrouve les déclarations/appel interne et l'appel test ligne68, pas de consommateur production de cet accesseur dans ce périmètre. Serena root ARTIST/cpp prêt vérifiée ; find_symbol ne voit que fallback lignes55–58 et références retournent `{}` : limite de couverture de la branche inactive, pas absence de consommateurs prouvée par LSP.

Deux observations auteur conservées hors correction actuelle : sstream non explicitement inclus ; oracle test fallback non conditionné pour GCC. Ne pas les transformer en défaut exécuté ni élargir le correctif sans décision.

## B4-U — scénario accepté pour proposition de tests

Nominal : trois tests historiques + cinq voisins ClosedExecution, noms/découverte à confirmer. Négatif/caractérisation : faute ASSERT fatale au marqueur unique dans une lambda ; capture étroite via ScopedFakeTestPartResultReporter existant ; owners vivants pendant oracle GL_CURRENT_PROGRAM ; garde de sécurité externe après l'oracle pour ne pas masquer le binding observé. Contrôle sans garde démontre le débind terminal sauté ; variante garde démontre cleanup avant destruction ; F10 conservé.

Le lead demande un **patch textuel test seulement**, sans édition des trois anciens tests à cette étape ni modification moteur/TestUtils. Exécution GL réelle par lead sous bail après revue. Nominal + caractérisation verte peuvent soutenir un refactoring pur ; ne pas fabriquer un RED moteur à partir d'une faute ASSERT intentionnelle. Tout défaut comportemental annoncé devra avoir sa preuve causale propre.

Recoupement symbolique lead : ProgramUnbinder récupérée, destructeur `glUseProgram(0)` ; références réelles aux cinq voisins ClosedExecution retrouvées. Aucun patch appliqué. Le worktree B4 étant détaché et son association à l'intégration Ensemble non prouvée, cette livraison intermédiaire est textuelle ; ne pas y créer de branche/commit ou y autoriser d'édition par supposition.

## Préflight runner réellement exécuté, sans installation

Le lead a relu `out/tooling/run-build.py` et `msvc-env.cmd` existants (non versionnés) : wrapper charge vcvars x64/toolset14.44 et journalise sous out/logs ; il ne lance pas pip/Conan install pour ces commandes. Présence vérifiée : python local, wrapper, vcvarsall VS18.

Dans cwd ARTIST, via Context Mode et `out/tooling/build/Scripts/python.exe out/tooling/run-build.py <label> <commande>` :
- label `qa-preflight-where-cl`, `where.exe cl` : exit0 ; premier chemin VS18/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe, puis installation VS2022 même toolset ;
- `qa-preflight-cmake-version`, `cmake --version` : exit0, **3.31.7** ;
- `qa-preflight-ninja-version`, `ninja --version` : exit0, **1.13.0.git.kitware.jobserver-pipe-1** ;
- `qa-preflight-conan-version`, `conan --version` : exit0, **2.12.2**.

Logs exacts : `out/logs/<label>.log` pour chaque ligne ci-dessus. Zéro configure/build/test/installation. La résolution de cl ne prouve pas encore une compilation ni CheckCxx23, la présence du contexte GL ou la validité d'un cache/binaire. Ces contrôles environnement ne sont pas RED.

## État et suite

Attente des propositions textuelles B4/B6 puis revue/persistance et exécution par lead sous mandat runner. H_Q/H_A acquis, permissions/accès non réputés corrigés pour B1/B3 par analogie. Aucune production changée, aucun GREEN ou PASS GPU/GCC ; campagne H3 NOT_RUN, C2 OPEN/INCONCLUSIVE et close réel DEFER.
