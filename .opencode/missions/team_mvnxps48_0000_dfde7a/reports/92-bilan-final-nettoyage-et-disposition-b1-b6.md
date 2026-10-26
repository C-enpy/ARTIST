# 92 — Bilan final : nettoyage stable livré et dispositions B1–B6

2026-10-26. Résultat attendu livré au candidat **a96104c7aaeede7dd6915bfeac86f53565615114** : chargement portable d'un fichier shader stable pendant lecture, contrôles conservés, acceptation du contenu après réussite, retrait méthodique des surcouches exploratoires. Aucune synchronisation pour écriture concurrente hors contrat, aucun nouveau backend/dépendance/service/outillage.

## Ce qui a changé

- Reader.hpp : callback et alias SourceExtractor retirés ; appel direct de l'extracteur. API publique `on`, mode texte, messages, read/gcount, masque/catch ciblés, scope propriétaire et close-vérification avant publication conservés. Aucun correctif comportemental spéculatif.
- ReaderTests.cpp : supprimer streambuf/global/adaptateur injectés et setter de publication dépendant de Windows ; trois tests correspondants retirés, trois caractérisations stables ajoutées. Préremplissage stale vérifié au rejet et au succès vide. Toutes fixtures isolées et régressions correctives conservées.
-13 fichiers sous scripts/probes/qa-f8-{stdio,close} retirés nominativement après contrôle des consommateurs ; aucun consommateur actif résiduel dans le périmètre production/scripts/CMake/CI contrôlé. Anciennes sources récupérables aux commits historiques, archives out/preuves/docs_meta/reports/research/worktrees/branches conservés. Wrappers partagés intacts.
- Photographie et commandes actuelles documentées ; ancien contrat seam marqué historique, pas réécrit. Diffs étrangers et stagedrename GrepAI non annexés.

## Validation acquise et limites exactes

Pré-refactor **15/15 Reader** sur ancienne production : caractérisations, pas RED fabriqué. Après refactor, normal **555/555** et ASan **555/555**, chacun484CPU+71GLréels,12Reader,0failure/error/skip/notrun. Découverte=sélection=exécution/JUnit,555uniques et même ordre ;17historiques chacun une foisASan. LastTest555passés,aucundiagnostictardifASan/runtime/troncature ; hashes sources/binaries/config/PDB/DLL inchangés après run.

Le nombre555 identique à l'historique masque un delta de population3retirés/3ajoutés : migration enregistrée, pas transfert de verdict. Préflight0/86 historique réutilisé après comparaison probe/summary/config et6DLL, aucun nouveaucanari. Commandes exactes,logs complets,JUnit et identités : rapport91 et `out/qa-f8/stable-20261026-01/`. Preuve portable `_meta/session-2026-10-26-stable-reader-cleanup.json`.

Revue native indépendante correctness-reviewer fresh reçue, session `ses_e864a6b90ffeWqUmBQrIGdpqpl`, puis audit des preuves : **aucun finding bloquant identifié dans le diff/les preuves examinés**. Reviewer n'a exécuté aucun test et n'a édité aucune source. Scénarios discriminants troncature,EOFexact,stale,NUL recoupés. Audit XML/selection/discovery/LastTest cohérent ;17historiques recoupés,absence de diagnostic dans extraitsfiltrés. Hashes attribués au runner, pas recalcul indépendant prétendu. Identité native/profil/parent/contexteneuf contrôlés ; certification complète rulesetprofil limitée. Ce n'est **pas** une requalification en profil test-reviewer.

## Gap-check local final (pas generator indépendant inventé)

- **KEEP** :12Reader courants, fixtures corrigées, consumers CPU/GL ; preuve normal/ASan et mutants mentaux reviewer.
- **MODIFY réalisé** : stale absent/vide, caractérisations1/4095/8192/8193 avecNUL/texte ; vert avant et après refactor. Aucun besoin de nouveau code de test découvert par l'audit.
- **REJECT** : nouveau framework/seam pour recréer injections, oracle Windows présenté portable, nouvelle calibrationOS ou sweepseed ; hors contrat opérateur.
- **GAP conservé, pas PASS** : erreurs authentiques extraction/close et non-publication sur close échoué ; observation dynamique portable de close-avant-publication. L'ordre est maintenu/revu statiquement. Aucun diagnostic d'impossibilité universelle ni disparition silencieuse d'obligation.
- Autres limites : GLcompilationaprèsNUL horsReader ; usagers externes des symbolesdetail non exclus ; GCC/Linux/Release/image/performance non exécutés. ADR0001/2/4/6/7 Proposed,0003/5 Accepted partial,0008 Windows Debug seulement.

## Disposition finale par branche initiale

- **B1 traité à portée bornée** : deux contaminations ON_CALL historiquement isolées et revue finale acquise. CompilationFalse :1échec/4historiques vs4/4current ; handles8/5 :2échecs/6historiques vs6/6current. Currentréduit au pin82bf52d,4fichiers pertinents inchangés à a96104c, vieux binaire rehashé identique. Overrides bornés EXPECT_CALL/WillOnce, pas resetexhaustif. Pas replay176/résolutionuniverselle, pas nouveaucorrectif nécessaire.
- **B2/B4/B6 acquis** : leurs correctifs/preuves restent liés à leurs candidats, anciennes tâches non rouvertes. Suitecourante normal/ASan acquise séparément après nettoyage ; aucune correction supplémentaire.
- **B3 stable livré** : contrat85+QA89 avant code, refactor90 et preuves91/revue finale. Poursuites stdio/close annulées NOT_RUN_OPERATOR_RECENTER, sans runtime. Gapsauthentiques ci-dessus explicitement conservés ; ce ne sont pas des critères nouvellement déclarés PASS. Chantierexpérimental fermé conformément à la décision opérateur, aucune reprise implicite.
- **B5/C2 disposition acquise** : fermeture administrative motivée acceptée au34 ; défautfixture établi, explicationforteEmpty/probableMultiBlock, primaires incomplets conservés ; pasnouvellechasse. Réouverture uniquement nouveau symptôme/preuvecontradictoire, pas absence d'archive ou silence.

## Organisation et état administratif

À la demande de reprendre sans nouvelles questions après proposition87/88 : tranche locale simplifiée menée jusqu'aux livrables et validations, QAplan/gap-check lead et revue indépendante ; aucune nouvelle permission/profile/config/code de harness. La chaîne des trois profilsQA natifs n'est pas prétendue exécutée. Les investigationsPM/architecte avaient été parallélisées ; revue indépendante en parallèle des builds/tests ; mutations et sorties sérielles pour ownership unique.

Deux tâches recherche PM/architecte complétées nativement **après intégration effective** de leurs remises. Anciennes affectations PC1_CLOSE/REVIEW_STDIO_PROBE et admissionQA incompatible restent **inactives/supersédées ou bloquées**, pas completedfictivement pour masquer un essai nonréalisé. Aucun outil exposé de cancellation/réaffectation approprié trouvé, aucune baseinterne éditée ; la dette d'affichage du board n'est pas du travail moteur restant ni un succès d'expérience. Aucune nouvelle tâche publiée/libérée, aucun worker de production actif à la clôture technique. Équipe et worktrees non purgés/archivés.

Le lot de nettoyage et les dispositions bornées de la reprise sont livrés. **Pas de PASS global moteur/harness** : les gaps techniques et limites administratives ci-dessus demeurent explicitement documentés. Plus de chantier OS/probes ou de question opérateur. Les contrôles documentaires et l'indexation du vault ont ensuite été achevés, sans modification du candidat C++ ni nouveau run cosmétique : voir rapport93.
