# 74 — Arbitrages regroupés avant source des outils d'observation

2026-10-26. Étape : deux propositions complètes intégrées, **aucun parent/enfant ni nouvelle sonde stdio écrits/exécutés**. HEADdocumentaire0a64e39, source moteur82bf52d inchangée. Préparations parallèles, auteurs/ownership disjoints ; aucune installation/service/traceur ou nouveau backend.

## Changements et preuves depuis68

- PC1_PARENT_RO5/5 intégré71/7b76245 : CLI/journaux/épilogue/supervision/callers réels/identitéCRT proposés, **limiteREDcomportemental explicite**. PolicycompileRED66/GREEN68 reste une preuve statique, pasREDduparent.
- S_PROBE_RO5/5 intégré72/0a64e39 : matrice30lignes, trois processusM0/m1texte/m1binaire avec M2recoveryinterne, observations/cleanup/oracles proposés. HelperRED58/GREEN62/revue63 acquis, ne couvrent pas automatiquement la future instrumentation.
- Gate baseline : Serena ciblée et comparaisonblob73 confirment extractShaderSourceinlineaccessible et fichierReader identique source82bf52d/canonique/worktreepool8. Appel direct production sur ifstreamfrais/badbitactif possible **sans copie de boucle, nouvelle seam ou modification moteur**. Complémentcourt demandé à l'auteur sur préconditions, pas nouvelle recherche nécessaire. Paspreuveexécutée du baselineprobe encore.
- Aucun test répété pour ce checkpoint ; gitdiffcheck ciblés/commitsdocumentaires seulement. Archives66/68 disponibles, aucun transfertASan555 au probe. B1revuefinale et auditfinalfresh restent à faire ; M5Reader/catch/nonpublicationouverts, B5/C2dispositionacceptée.

## Conflit de méthode explicite, pas autorisation implicite

Le contrat général impose RED→GREEN avant implémentation ; les nouveaux outils de mesure n'ont pas encore de RED comportemental honnête. Le parent absent ne fournit pas une baseline runtime ; ajouter un squelette volontairement faux, des flagsstream, handles invalides ou un helper décoratif fabriquerait une preuve. La policycompileGREEN n'exempte pas le superviseur. Les résultats CRT inconnus sont déjà des caractérisations admises46/52, mais cette admission ne couvre pas tacitement toute la logique nouvelle de l'instrumentation.

**Option A, recommandée pour cette investigation bornée** : accepter explicitement un écart ciblé au RED comportemental préalable pour les seuls nouveaux outils d'observation parent/enfant et sonde stdio, avec oracles/invariants écrits avant logique, exécution nominale/négative pertinente par runner lead et revue sensible fresh, GAPREDcomportemental conservé dans les preuves. Contrôles déterministes testés d'abord quand une tranche causale honnête est disponible ; pas création de défaut pour rougir. Toute correction révélée ensuite garde son vraiRED/oracleinchangé/GREEN. Ce choix ne vaut pas TDDcausal accompli, ni garantie de comportement non testé, ni exception pour moteur/Reader ou changement futur hors ces outils.

Compromis : permet de mesurer ce qui reste inconnu sans campagne de nouveauxhelpers juste pour afficherRED, mais la couverture causale préalable du nouveau superviseur/instrumentation demeure absente et doit rester visible. Avant source : décision explicite/amendementQA commis et GOlimité ; avant essais : source intégrée/argv/SHA/sélections/bail vérifiés, runnerleadseul, stop sur surprise. Aucun abandon de catch/close ni PASS global.

**Option B** : maintenir le gateRED pour cette logique et demander une conception testable/scénario causal supplémentaire en RO, sans code. Plus strict sur preuvepréalable mais ne garantit pas qu'un scénario honnête existe sans nouvelle surface ; pas framework/seam/infra supplémentaire autorisé par ce choix. En cas d'absence persistante : retour au choixopérateur, pas codebloqué renommé terminé.

## Matrice stdio et détails locaux proposés

Adopter ou réviser la recette72 :10vecteurs×3classeschemin=30lignesM0, sans sweep ; M1texte/binaire chacun une observationnative33/fread puis recoveryfluxneuf. Corpsproductiontexte direct73, pas nouvelleloopprésuméeéquivalente. Issues0favorable/10défavorable/20précondition/30instrumentationcleanup, lignesconservées et agrégation30>20>10>0. ReprésentabilitéACP absente reste20, jamais skip/PASS. Vecteurs/offsets/suffixes exacts et testscomparaison/agrégation avant logique, helper/controlsvalidés immuables. AucunM3/modeReader/CLIclose adoptés automatiquement.

Parent : détails71 restent soumis à validationlocalelead après décisionméthode. En particulier résultatinstrumentation complète et hypothèserefusclose doivent être **deux champs distincts** ; fclose0 sousprotection ne confirme pas le refus et ne ferme pasM5 même si logs complets. Aucun statut0instrumentation ne sera résumé en PASSclose. ConserverHOLDsanskill et limitecrashhôte, CLI/codesfinals à figer avant codeparent.

## Décisions exactes demandées

1. Accepter explicitement la stratégie bornée de caractérisation/invariants des seuls outils avec GAPREDcomportemental conservé, ou maintenir le gate et demander conceptionRO supplémentaire ? Le conflitTDD est exposé, aucune exemption silencieuse n'est appliquée.
2. Adopter la matrice stdio précise72/voiebaseline73/issues, ou demander sa révisionRO ? Aucun essai supplémentaire hors cette sélection ni modification Reader autorisés dans ce choix.

Après réponses : persister décision et amendementQA ciblé avantGOsource, poursuivre branchesdisjointes en parallèle puis builds/runslead sérialisés. Scoresdéveloppement79/84close,70/66stdio et routage approuvé inchangés ; pas nouveau modèle/provider/permission ni relance d'enquêtehistorique.
