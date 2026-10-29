# QA Linux — mêmes trois échecs sous ASan, populations recoupées

## Objectif et résultat actuel

La campagne Linux sous ASan est terminée et reproduit les mêmes trois échecs d'attributs que la configuration normale. La parité QA reste **FAIL**, sans exclusion ou skip. La réussite du canari ASan ne transforme pas les échecs fonctionnels en réussite.

Le lead a relu les artefacts existants, sans relancer compilation ou test :
- Normal : **506/506 CPU**, **72/75 OpenGL**, zéro error/skip.
- ASan : **506/506 CPU**, **72/75 OpenGL**, zéro error/skip.
- Chaque campagne contient581noms uniques ; les ensembles de noms bruts normal/ASan sont identiques. Les trois failure sont exactement les mêmes : préflight d'orthographe, Reader array1, refus array avant mutation du leurre.
- Le summary du canari préalable ASan indique sain0, faute volontaire86, politique canonique et statutPASS. Cette preuve est limitée au canari ; elle ne prouve pas l'absence globale de défaut mémoire, et `detect_leaks=0` reste explicite.

L'auteur indique llvmpipe dans tous les contextes JUnit ; cette caractérisation n'est pas une preuve de GPU matériel ou de performance.

## Identité des sources : écart de fins de ligne expliqué

Le lead a comparé les148identités de fichiers source disponibles dans chaque manifeste au checkout canonique. Deux hashes bruts différaient : `ShaderTests.cpp` et `FreerTests.cpp`.

L'expérience discriminante ciblée confirme que les deux fichiers Linux sont **strictement identiques aux blobs Git du candidat a4c0f82**. Les copies Windows ont respectivement173 et70fins de ligne CRLF ; après la seule conversionCRLF→LF, les contenus sont identiques. Les146autres hashes concordent bruts. Il ne s'agit donc pas d'une modification de test dissimulée, et aucune réécriture des fichiers n'a été faite pour uniformiser artificiellement leurs hashes.

Les binaires Linux et le helper compagnon encore non intégré ne sont pas comparés aux fichiers Windows. Leurs identités et leur livraison restent à vérifier à l'intégration ; une correspondance de sources ne remplace pas cette barrière.

## Helpers et travaux indépendants

L'auteur annonce un helper compagnon Python utilisant la bibliothèque standard pour CTestJSON/JUnit, réutilisant le lanceur et les presets existants, avec sorties uniques. Quatre tests auxiliaires sont annoncés verts sous Windows, dont propagation d'un enfant en échec et refus d'une ancienne sortie. Ce sont des résultats auteur non encore intégrés/rejoués par le lead.

Les manifestes initiaux des deux campagnes portent encoreINCONCLUSIVE et n'enregistrent que le compteCPU. Les JUnit établissent pourtant les trois failure : le verdict consolidé demeureFAIL. Le lead a demandé une correction testée du helper pour conserver les comptes graphiques même après retourCTest non nul et donner priorité auFAIL, sans écraser les archives initiales.

La reproduction CI distante reste autorisée sur une branche de mission, jamais par poussée ou merge de main, modification de protections/secrets ou ajout de runner auto-hébergé. Elle n'est pas encore acquise. La sonde comparative scalaire/array1/array2 reste le prérequis du diagnostic graphique, sans patch de production. Les ressources distante et Linux sont distinctes et peuvent avancer en parallèle ; pas de compilation concurrente dans un même arbre.

## Suite et limites

Attendre les résultats de sonde et la livraison workflow/helpers pour décider sur preuves. Consulter l'architecte avant toute nouvelle provenance de métadonnées ou changement du contrat public. Gaps, audit et clôture restent différés jusqu'à intégration réelle. Aucun contrôle non exécuté n'est promu enPASS.

## Annexe de preuves

Racine : `/home/artist/src/ARTIST-integration/out/linux-evidence/`.
- Normal : `20261028T172225Z-4e352cedae774fe395f9ebf7ea6ccfef/`.
- ASan : `20261028T174257Z-c22318dc066f48559194a828508879d7/`.
- Artefacts relus : manifests, unit.xml, opengl.xml et canary/summary.json ASan ; lecture seule PythonXML/JSON via Context Mode.
- Comparaison ciblée : fichiers Linux via lectureWSL, fichiers Windows et `git show a4c0f82:<chemin>` ; seule normalisationCRLF→LF pour qualifier l'écart, pas pour remplacer les hashes bruts.
- Une erreur de syntaxe dans le premier script de comptage de fins de ligne a été corrigée avant toute lecture probante ; aucune panneMCP ou échec de test n'en est déduit.
- Production et testsC++ inchangés par ce jalon. PreuveWindows antérieure conservée au candidat indiqué, sans relance inutile.
