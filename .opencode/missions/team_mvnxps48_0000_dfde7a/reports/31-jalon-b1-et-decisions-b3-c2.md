# 31 — Jalon B1 et décisions B3/C2

2026-10-25. Scope INITIAL sous révision3, long_run/manual. Résultats nouveaux vérifiés, pas une clôture depuis le board de continuation.

## Changements et preuves

**B1 : progrès causal exécuté.** Deux réductions séparées sur H historique identifié vs N actuel identifié, sélection29 pré-run : A_H exit1/1failure et A_N exit0/4RUN ; B_H exit1/2failures et B_N exit0/6RUN.5cas uniques répétés2fois par binaire, XML dernière itération distinct du compteRUN. Causes ON_CALL compileFalse et handles8/5 persistants isolées ; sites déjà EXPECT_CALL/WillOnce dans le code actuel. Rapport30 et archives complètes out/qa-b1/causal-20261025-01. Aucun correctif/build/sweep/GPU/ASan nouveau. Revue finale encore à faire.

**B5/C2 : nouvelle provenance positive**, rapport28/transport auteur précis. Défaut de fixture contemporain établi : dossier parent non créé, écriture non vérifiée, TearDown supprime dossier ; garde exists=false avant Reader. Edit create_directories, environnement réappliqué/lien puis red-exact11/16 sans deux gardes Reader en échec. Relu par b5-provenance : explication fortement étayée pour Empty, probable pour MultiBlock ; valeurs complètes de MultiBlock/XML/binaires pré-run manquent. Un dernier contrôle borné des seuls passages publics de cette session sur ctest.xml/baseline n'a pas retrouvé ces valeurs ; pas de nouvelle chasse ni replay.13baseline et11focus restent populations différentes. Recommandation E5 : fermeture motivée explicite de l'enquête, sans dire moteur corrigé/causalité exhaustive ; alternative OPEN sur manques nommés, pas DEFER silencieux.

**B3 : contrat architecte complet reçu**, b3-contract-proposal.md. Stdio fread/ferror/feof/fclose proposé pour distinguer erreur et EOF, publication après lecture ET close réussis, modes texte/chemins/messages à caractériser, owner local unique/close une fois. L'ancienne obligation catchifstream authentique serait remplacée, non exercée, si cette migration est adoptée. Aucune preuve authentique de close échoué identifiée avec les fixtures régulières lecture seule actuelles. Aucun correctif ni essai architecte effectué.

## Options et recommandation

**B3 maintenant : calibration seule recommandée**, sans adopter prématurément un remplacement de branche. AmendementQA préparé sur proposition ; sondeexécutable dédiée seulement après contratsonde/planQAcommité, C≥70, runnerlead. Vérifier erreur réelle sous verrou et compatibilité texte/chemins ; rechercheOS de close authentique bornée en parallèle, sans installation/dépendance/handle invalidé artificiellement. Après résultats : adoption détaillée du contrat Reader et substitution éventuelle du catch soumises explicitement. Si stdio perd aussi l'erreur, le rejeter et comparer Win32, pas heuristique. Alternative : étudier Win32 directement, plus de surface sémantique ; ne pas modifier Reader maintenant.

**C2 : fermeture motivée bornée recommandée** sur provenance contemporaine retrouvée, et non absence d'archives. Qualification : défaut de fixture établi, explication forte pour Empty/probable pour MultiBlock, reconstruction complète non prouvée ;13échecs et SHA documentaires conservés, réouverture sur primaire contradictoire ou nouveau symptôme identifié. Alternative : conserver OPEN/INCONCLUSIVE, propriétaire lead+build-engineer, déclencheur remise identifiée des sorties/identité manquantes ; aucune nouvelle expérience actuelle ne restitue rétroactivement le binaire perdu.

Les décisions ne ferment pas B3catch/close ni ne promeuvent ADR. B2/B4/B6 restent acquis au candidat82bf52d. Une nouvelle source exécutable imposera nouvelles preuves proportionnées, pas recyclage d'ASan555.

## Décisions demandées

1. Autoriser la calibration stdio et la rechercheOS close bornées, **sans correction Reader ni abandon catch/close à ce stade**, ou choisir étude Win32 directe ?
2. Accepter la fermeture motivée bornée B5/C2 avec ces limites, ou maintenir OPEN en exigeant les primaires identifiés manquants ?

Pendant ces décisions : QA peut préparer son amendement documentaire, sans worktree auteur ni changement source. Aucune nouvelle autorisation modèle/provider ni pauseopérateur levée.
