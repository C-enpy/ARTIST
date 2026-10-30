# Plan QA ratifie — pipeline uniforms Windows/Linux

Version 1 ; contrat contract.md au commit 003ad9c5804a31523b46150579589abba3a30f65. Auteur de la proposition : qa-planner/pool-2, session ses_e7626883effe2VyjXAO2qEvRRk, reponse finale publique msg_189da1351001zr1oaU6F5shSug (15562 caracteres). Complement natif result_mvu4zzei_000l_84xjul. Persistance et decisions locales : coordinateur.

Le plan est propre a mission-uniform-pipeline ; aucun ajout au plan historique mission-gpu-qa suggere par l'auteur n'est effectue. La remise complete a ete retrouvee par le lead via l'API native de lecture de messages publics (parties text seulement), pas par inspection du raisonnement prive. Une nouvelle enquete QA n'a pas ete necessaire.

## 1. Admission et perimetre

Le profil natif servi a ete verifie avant publication ; correction minimale des droits autorisee par l'utilisateur, edit/shell/subagent deny et lecture/restitution disponibles pour les trois QA. La lecture locale AGENTS, skill, contrat, qa-inputs et politique est confirmee par le lecteur ; team_message puis final_response ont ete recues. Cela ne prouve pas de sandbox.

QA proportionnee ; aucun changement de catalogue ni de score pour satisfaire la QA. TDD et revues du projet conserves. Autorise apres approbation native : scripts et processus factices, donnees synthetiques seulement. Interdit : moteur meme CPU, compilation moteur, CMake/Conan/CTest, rendu logiciel/materiel, benchmark, installation, distant et cloud. Workloads C++/GL livres NON_EXECUTE, sans RED/GREEN graphique reconstruit.

Les auteurs possedent leurs tests, le lead est runner des preuves retenues et distingue auteur/proposition/executant. Les propositions QA ne sont pas des commandes a executer automatiquement. Contrat et ce plan commites avant worktrees auteurs.

## 2. Obligations explicites et controles inferes

Explicites (E) :

- E1 : effets bornes ; list/plan purs et report en lecture seule.
- E2 : catalogue ferme et selections explicites avec producteurs reellement raccordes.
- E3 : prepare/build/run/report separes, pas de phase implicite ni de telechargement en qualification.
- E4 : plan/candidat/scripts/config/shaders/binaires/environnement attribues et conserves.
- E5 : CPU seul, logiciel et materiel distincts ; ambiguite UNVERIFIED, aucun repli silencieux.
- E6 : etats pipeline, tests, validite mesure et fraicheur separes ; aucune absence ou erreur convertie en succes.
- E7 : scenarios graphiques reels et limites NON_EXECUTE ; pas de gain requis.
- E8 : RED causal, GREEN et non-regression synthetiques ; integration avant consommateurs et audit fresh.

Inferes (I), controles servant les obligations, sans nouvelle fonctionnalite :

- I1 : doubles de processus/outils qui echouent immediatement sur tout appel reel non factice.
- I2 : relecture du meme bundle apres mutation pour eviter reclassification favorable/cache stale.
- I3 : contre-exemples/mutants synthetiques cibles, notamment code0 sans resultat et evenement manquant.

## 3. Risques QA (I et P chacun 1–5, distincts des scores PM)

R1 Faux resultat favorable : I5 P4 =20. Declencheur : code0 avec resultat absent, selection vide, omission/doublon ou mesure invalide. Portee : conclusions du pipeline. Detection actuelle : contrat et garde historique reconcile relue. Mitigation : etats independants et populations exactes. Gap : tests synthetiques des contre-exemples et priorites agregees.

R2 Identite/peremption : I5 P3 =15. Declencheur : shader/binaire/config/identifiant modifie ou ancien receipt. Portee : attribution de toutes preuves. Detection actuelle : collecte historique d'empreintes relue statiquement. Mitigation : relevés avant/apres et receipts lies au plan. Gap : mutation de chaque categorie et coherence du nouveau bundle, incluant fichiers non suivis pertinents et chemins sortants.

R3 Renderer substitue : I5 P3 =15. Declencheur : inconnu/logiciel accepte comme hardware. Portee : qualification materielle et interpretation mesures. Detection actuelle : garde llvmpipe historique seulement. Mitigation : profil explicite et UNVERIFIED. Gap : matrice synthetique demande/observation/cible attendue, contradiction et absence de preuve.

R4 Effets implicites : I5 P3 =15. Declencheur : import/list/plan sonde ou ecrit, fallback runner reel quand une doublure manque. Portee : machine locale et interdictions de mission. Detection actuelle : inspection helpers, dont les modes usuels peuvent lancer build/CTest. Mitigation : runner injecte et phases distinctes. Gap : sentinelles processus/reseau/ecriture/import et CLI reelle en contexte factice, bytecode compris.

R5 Mauvais raccordement : I4 P3 =12. Declencheur : build sans receipt valide, MSVC perdu ou helper execute ses tests implicitement. Portee : portabilite et separation phases. Detection actuelle : code helpers relu. Mitigation : adapters distincts et commandes declarees. Gap : commandes/env/quoting/codes sous doubles et verification des references CMake/CI en lecture seule.

R6 Documentation ambigue : I2 P3 =6. Detection et mitigation : revue ciblee des exemples, scopes et statuts ; pas de campagne exhaustive pour ce risque seul.

Reevaluer apres integration ou contre-exemple ; aucune hausse artificielle de l'impact pour compenser une faible couverture.

## 4. Scenarios synthetiques et points de preuve

S1 Purete/catalogue (E1,E2,I1) : nominal import/aide/list/plan selection valide ; negatif inconnu/doublon/vide et toute tentative enfant/reseau/ecriture. Oracle : sortie deterministe et zero effets, jusque bytecode. Cibles : verify.py, catalogue.list_scenarios, planning.make_plan. Un test dans une copie en chemin avec espaces detecte les fichiers crees ; audit des imports et sentinelles pour appels interdits.

S2 Schemas/bornes (E2,E4,E6) : aller-retour JSON, tuples et hashcanonique ; negatif champs inconnus, parametres0/negatifs/au-dela limite, batch invalide, hash divergent, doublons flags et mauvais enum. Oracle : rejet causal avant commandes. Modeles et workload CLI doivent employer les memes limites.

S3 Phases/receipts (E3,E4,E6) : chaine valide sous doubles ; negatif prepare manquante, build manquant, receipt autre plan/run ou vieux dossier. Oracle : refus, pas de reconstruction implicite ni de phase executee en remplacement. Cibles : evidence.prepare/build/execute et adaptateurs.

S4 Runner/erreurs (E3,E6,I1) : processus factice succes/logs ; negatif nonzero, absent, timeout, interruption, sortiepartielle, faux runner manquant et journal long. Oracle : code brut/logs preserves, arret des seuls enfants possedes, aucune fuite vers outils reels. Cibles : runner.ProcessRunner.run et evidence.

S5 Resultats/populations (E2,E6,I3) : ensemble exact de resultats, selections/discoveries/JUnit synthetiques ; negatif code0 sans JSON, resultat tronque, nom/parametres/run incorrects, omission/doublon, skip requis, comptes faux. Oracle : jamais succes ; toutes les causes restent visibles et priorites CLI concordantes. Pas de nombres historiques codes en dur.

S6 Identite (E4,I2) : fichiers sources/config/shaders/binaires coherents ; negatif mutation separee de chaque categorie, ancien receipt, chemin hors bundle/symlink sortant et relecture cachee. Oracle : STALE ou UNVERIFIED sans preuve favorable. Pas de promesse de detection d'une mutation transitoire annulee.

S7 Profils/adaptateurs (E3,E5) : CPU sans GL ; logiciel llvmpipe explicite Linux ; hardware attendu sous observations synthetiques concordantes. Negatif logiciel pour demande hardware, inconnu/absent/contradictoire et logiciel Windows non supporte. Oracle : refus ou UNVERIFIED, pas hardware par simple exclusion. Verifier Conan avant CMake, typebuild coherent host/build, preservation historique, MSVC lifetime, chemin espaces et restauration, uniquement doubles.

S8 Mesures (E6,E7) : echantillons synthetiques, populations/unites coherentes, effet nul ou negatif recevable ; negatif temps ecoule negatif, timer fige/sous-resolution, unite incoherente, NaN/inf, sample manquant et oracle faux. Oracle : validite non favorable si mesure demandee invalide ; valeur absente distincte de0 ; un effet negatif n'est pas une panne pipeline. Aucun temps CPU nomme GPU. Protocole statistique ne force pas l'obtention d'un gain.

S9 Scenarios reels (E2,E7) : revue statique d'un update utile puis draw par cellule, retour0/.5/0, images fixes non vides, miroir/readback, garde programme/contexte/revocation et controle de travail hors chrono. Negatifs a livrer/referencer : update/draw omis, valeur figee, decoy, ancien acces. Ces scenarios graphiques sont relus, jamais executes ici.

S10 Rapport/CI (E1,E4,E6,E7) : relecture autonome des preuves existantes ; negatif manques/interruptions/priorites concurrentes et rapport ancien. Oracle : rapport n'execute aucune phase, ne cree pas de preuve et conserve les manques. Workflow exclusivement manuel, cibleoptionnelle hors suites usuelles, pas de secrets/interpolation libre ni campagne implicite.

## 5. Registre bidirectionnel et preuves

- verification_core : S1–S6, S8, S10 ; E1/E2/E3/E4/E6/E8.
- verification_windows : S3/S4/S7/S10 ; E3/E4/E5/E8, specificites MSVC et quoting.
- verification_linux : S3/S7 ; E3/E4/E5/E8, Release/ABI et politiques llvmpipe preservees.
- verification_scenarios : S2/S5/S8/S9 ; E2/E6/E7/E8, generateurs/parseurs synthetiques et raccordements statiques.
- Revue des fichiers C++/CMake/shaders et selections tests existants : S9/S10 ; E2/E7. Pas de preuve runtime.
- Revue independante finale : tracabilite E1–E8, mutants discriminants avant findings hauts, correction propriétaire et re-audit.

Chaque assertion/tampon de preuve doit pointer obligation et scenario ; aucun test orphelin ajoute pour faire monter les totaux. Tout item non couvert est GAP ou DEFER explicite, jamais PASS implicite. Les tests historiques relus sont de la presence statique, pas des executions du nouveau candidat.

Enregistrement de chaque preuve executée : auteur/proposition/runner, HEAD/delta et empreintes, commande/cwd/interpreteur, attendu/observe, tests decouverts/exécutes/failures/errors/skips et extrait causal RED/GREEN. Zero test, import casse accidentel ou outil absent n'est pas RED attendu. Les preuves RED doivent precéder implementation de la tranche.

Les noms test_contract.py sont proposes pour la premiere tranche de chaque branche ; les commandes retenues seront celles des fichiers reels. Commande cible par branche : C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_<branche> -p 'test_*.py' -v, dans son arbre isole, via runner lead et Context Mode. -B pour ne pas produire de bytecode. Aucun fichier de test inexistant ne sera pris pour une execution.

Avant lancement : relire imports, top-level et confinement du runner/doubles. Les seuls executables enfants permis sont l'interpreteur Python et les petits programmes factices possedes par ces tests ; jamais appels reels de helpers build-windows/build-linux/linux-evidence, CMake/Conan/CTest, GL/workloads ou reseau. Mesures et controles de plateforme non realises restent NON_EXECUTE.

## 6. Bornes ratifiees par le lead avant auteurs

Iterations : 1 a 1000000 ; repetitions : 1 a 100 ; batch_size : 1,16,256 ; produit iterations*repetitions*batch_size <=100000000. Timeout commande : >0 et <=3600 secondes, defaut60. Rejeter overflow/valeurs non entieres avant allocation ou lancement. Ce sont limites de l'outil, pas autorisation de campagne ni budget cloud. Toutes bornes figurent dans le plan et sont controlees dans les parseurs de workloads livres.

Pas de seuil de gain fixe. Le faux A/A et les limites de bruit sont des donnees du futur protocole ; leur qualification reelle reste hors mission. On teste ici uniquement la gestion correcte des samples et des verdicts synthetiques.

## 7. Charge et arret

Allocation relative proposee : setup25% ; du reste,75% pour controlesplanifies et25% buffer investigation/panne/re-audit. Le budget horaire indicatif de l'auteur QA n'est pas adopte comme engagement ni limite native de cette mission. Prioriser risques>=10 ; couper extensions et cosmétique avant de supprimer leurs controles.

Arreter le controle affecte en cas d'appel reel imprevu, reseau/bootstrap, mutation hors fixture, candidat divergent, code0 sanspreuve, renderer substitue, logperdu ou oracle affaibli. Conserver les preuves, diagnostiquer sans elargir droits et poursuivre les branches independantes autorisees. Pas de repetitions plus couteuses sans hypothese nouvelle.

## 8. Reception et barrieres

Admission QA qualifiee par regles natives, lecture et restitution ; ce plan et contrat commites avant auteurs. Chemins/ownerships du contrat prioritaires sur les suggestions anterieures des tâches. Les auteurs presentent leur premiere tranche de tests et son attendu avant implementation ; le lead observe RED causal, puis permet GREEN minimal et non-regression.

Apres integration reelle des quatre producteurs : integration des raccordements ; puis generator des gaps demontres (texte/patch, jamais production modifiee par QA), corrections integrees avant audit fresh. Les consommateurs restent deferred avant leurs entrees effectives.

Sortie : tests synthetiques attribues et concordants, catalogueproducteurs reellement raccorde, rapporthonnete des manques et revue des changements sensibles. Pipeline livre pour qualification Windows/Linux ; execution reelle des workloads, compilation C++, images et performance NON_EXECUTE. Aucun gain exige. Une future qualification ne conditionne pas la livraison ici, ni n'autorise du cloud.

## 10. Addendum ratifié — contextes historiques v1

Proposition QA native `result_mvucmc0f_001x_7w875k`, profil qa-planner fresh/pool-9. Contrat : `legacy-context-observation-v1.md`, entrée commitée `452b615d9efbb89e8eab93a9181ee6c017255684`. Lecture locale et restitution finale natives confirmées. Une lecture a visé par erreur `evidence/.../qa-inputs.md` absent ; le vrai contexte était fourni dans le dossier de mission. Pas de permission élargie, de nouvelle tentative ou de besoin de cette digestion pour les sections réalisées. Les sources et contrats lisibles suffisent à l'addendum ; cette limite est conservée.

Exigences explicites : activation absente inchangée / partielle refusée ; arrêt avant corps et propagation au SetUp dérivé ; une observation attribuée par cas OpenGL ; six variables publiques issues du plan ; parser pur, populations et statuts distincts ; négatifs/ownership et modes consultatifs inchangés. Contrôles inférés : sentinelles d'effets, ordre d'agrégation permuté et mutations d'un seul champ. Pas de nouvelle fonctionnalité ni nouveau seuil de gain.

Top risques I×P : arrêt non propagé (5×4=20), faux favorable attribution/agrégation (5×4=20), transport divergent/activation implicite (4×3=12). Déclencheurs : retour base suivi de GL dans SetUp dérivé ; ligne absente/double/étrangère ou précondition masquant assertion ; variables héritées/paire incomplète/Unicode normalisé. Portée : trois cas OpenGL historiques et rapport de sélection, pas conversion des cas CPU en preuve GPU. Détection actuelle : lecture source/contrats seulement. Mitigations : garde locale avant travail et contrôle du SetUp appelant, identités strictes par cas, transport explicite et chaîne exacte. Gaps : revue du nouveau flux natif et nettoyage ; tests parser/JUnit mixtes et codes ; tests adaptateurs/Unicode. La compilation/exécution GL demeure hors preuve même après clôture synthétique.

Groupes additionnels et oracles :

- A1 (activation/compatibilité) : toutes variables absentes historique ; chaque champ partiel/invalide refuse sans repli. Revue statique native.
- A2 (arrêt/lifetime) : contexte possédé 3.3+ core et cible exacte avant programme ; refus courant/version/masque/logiciel avant corps et boucle dérivée. Négatif de détachement ultérieur conservé. Revue indépendante statique, pas simulation transformée en preuve native.
- A3 (parser/attribution) : ligne unique JSON stricte ; absence/doublon/troncature/clés dupliquées/types/version/identité/nom incorrects : code protocole 4 au collecteur, même enfant 0.
- A4 (transport) : six valeurs exactes, identité distincte par invocation ; Unicode 4096 scalaires et espaces conservés, paire/contrôle/NUL/surrogate/4097 rejetés, quotes/backslashes non interprétés.
- A5 (population) : noms découverte/sélection/JUnit concordants, observation sur chaque OpenGL matériel, pas exigée sur CPU ; omission/duplication/transfert entre cas refuse un favorable.
- A6 (priorités) : précondition structurée cohérente →3 ; assertion réelle + précondition ailleurs →5 avec causes/population partielle ; protocole + assertion →4 ; interruption →130 ; skip ordinaire ou enfant incompatible jamais favorable. Permutation de l'ordre stable.
- A7 (pureté) : descriptions et list/plan sans sonde, écriture, enfant ou import workload ; environnement contradictoire ou doublure manquante ne déclenche aucun fallback.
- A8 (relecture) : parsing de deux cas sans état partagé, mutation identité/JUnit relue sans cache ni relance.

Registre bidirectionnel : fixture commune et SetUp dérivé →A1/A2 ; nouveau parser `context_evidence.py`/tests dédiés →A3/A4/A5/A8 ; adaptateurs describe_execution →A4/A7 ; collecteur/rapport →A5/A6/A8 après intégration des producteurs ; catalogue →A5 avec présence statique des trois noms. Chaque groupe A1–A8 doit pointer ces obligations ; tests de charge/mesure du plan initial restent hors delta, pas artificiellement orphelins. Aucun nouvel item n'est encore COVERED. Les 31 tests workload et les 124 succès/1 skip du cœur ne prouvent aucune de ces nouvelles gardes.

Allocation relative additionnelle : setup25 ; du reste75% planifié (56,25) et25% buffer (18,75), sans promesse horaire. Priorité aux contre-exemples des risques élevés avant cosmétique. Arrêt du contrôle affecté si enfant réel non autorisé, réseau, mutation hors fixture, candidat divergent ou oracle affaibli ; poursuite des branches indépendantes.

Séquence obligatoire : contrat/interface/ownership + cet addendum committés → auteur parser/producteur → RED/GREEN synthétiques par runner lead → intégration effective → collecteur/adaptateurs → non-régression et revue indépendante native fresh, corrections/ré-audit. L'auteur n'exécute pas de tests. Aucune édition implicite des autres fixtures dérivées, corps de tests, assertions, moteur ou CMake. Gaps de stratégie/ownership du candidat QA sont levés par l'interface fixée et l'affectation explicite du lead, pas par une nouvelle permission.

Sortie additionnelle : A3–A8 exécutés sur données/processus factices par le lead, A1/A2 revus indépendamment, codes/logs bruts et limites conservés, aucun finding haut ouvert sans décision. Livraison pour qualification ; C++ et GL NON_COMPILE/NON_EXECUTE, jamais fonctionnement démontré.

## 11. Addendum ratifié — bundle Windows et préparation pure

Proposition qa-planner/pool-21, session `ses_e74b2e63bffe2V2PnsBSflHrRv`, réponse finale publique complète `msg_18b4ddef4001bn4XbXogv09VOP` (11656 caractères), rectification native `result_mvujhmp8_004p_513f22`. Le lead a récupéré la réponse publique par API native, parties text uniquement, sans lecture du raisonnement privé. Persistance, arbitrages et exécutions futures : coordinateur.

### Admission, pin et limite documentaire

Profil servi et routé qa-planner vérifié avant départ, lecture locale et `team_message` réussis, réponse finale complète reçue. Aucune édition/test/build par le planner, aucune garantie de sandbox. HEAD fourni au lecteur : `493bf4f73a8ba5f8f1b7350deb7aaab09fcd1b3b`, non vérifié par Git par ce profil restrictif.

La référence `qa-inputs.md` absente était une erreur du brief lead, pas un refus de permission ni un blocage du lot. La rectification du planner confirme qu'aucun oracle du delta prepare/bundle/payload/receipts n'en dépend. Les sources locales, `qa-bundle-inputs.md`, le binding ratifié et le plan existant suffisent ; aucune lecture intégrale du corpus pool-5 absent n'est revendiquée.

Le code observé utilise encore `describe_preparation(plan)` : le keyword bundle_root est futur. Les statuts d'implémentation partial observés au registre du29octobre ne changent pas les décisions Proposed/Accepted. Les preuves de fondations sont réutilisées, pas rejouées pour cet addendum : cœur170succès+2symlinkSKIP/172, contexte28, scénarios31, Windows51, LinuxRelease6+19. Leurs commandes, empreintes et exécutant lead sont dans les rapports81/82/90 et manifestes référencés ; aucune ne prouve le nouveau delta.

### Obligations explicites B1–B6

- **B1 — Pureté et bundle explicite.** `describe_preparation(plan, *, bundle_root: str)` reçoit le chemin absolu admis sous lease. Linux et doubles acceptent le keyword ; aucun fallback après TypeError, y compris TypeError interne.
- **B2 — Transport possédé par le sink.** Création exclusive de `raw/windows-helper-input.json` avec exactement `build_directory`, `targets`, `executable: ""`, `arguments: []`. Root issu de `_build_root`, union ordonnée dédupliquée des cibles du plan. Fermeture, SHA256 et inscription artefact précèdent toute commande.
- **B3 — Réemploi sans réparation.** Build réutilise les mêmes octets depuis le PreparationReceipt admis. Collision, disparition, mutation, reçu étranger ou incomplet refusent avant lancement, sans réécriture réparatrice.
- **B4 — Phases et effets bornés.** Descriptions/list/plan purs, répertoires de logs privés prepare/build créés exclusivement, opt-in de préparation respecté avant helper/bootstrap, aucune construction ou exécution implicite. L'opt-in concerne la résolution explicite des dépendances via `allow_dependency_resolution` ; ne pas transformer les doubles génériques sans bootstrap des contrats existants en commandes réelles ou les affaiblir pour le raccordement.
- **B5 — Conservation et attribution.** Bruts/artefacts partiels conservés ; interruption/échec ne donne aucun reçu favorable. Capture commune primaire, pas deuxième capture.
- **B6 — Compatibilité.** Request, CommandSpec, schémas de receipts et signature build inchangés. Consommateurs adaptés après intégration du core seulement.

Contrôles inférés, non fonctionnalités : sentinelles d'effets et compteurs (I1), interruptions injectées aux frontières de publication sans promesse de toutes pannes système (I2), permutation de deux bundles du même plan pour détecter setter/global ou « dernier bundle » (I3).

### Top trois risques impact×probabilité

- **Attribution/réemploi :5×4=20.** Déclencheur : bundles du même plan, reçu échangé, mutation entre phases ou ancienne API masquée. Portée : commandes/preuves du mauvais candidat. Détection actuelle : `_predecessor` et empreintes, présence source seulement. Mitigation : bundle exclusif et reçus run/plan. Gap : transport Windows absent, T1–T3 à qualifier.
- **Effets/collisions :5×3=15.** Déclencheur : matérialisation dans description, opt-in tardif, destination préexistante ou chemin interne réservé. Portée : preuves existantes/arborescence/interdictions. Détection : contrats de pureté et `_within`. Mitigation : lease et création exclusive. Gap : confinement et hash seuls ne prouvent pas l'ownership, T2/T4/T5.
- **Publication/interruption :4×3=12.** Déclencheur : interruption pendant écriture, après fermeture avant hash, après hash avant commande ou pendant fake command. Portée : reçu et preuves. Détection : `_cause`, tests d'interruption existants. Mitigation : états distincts et logs. Gap : ordre de publication nouveau non testé, T2/T6.

Réévaluer la probabilité après intégration/contre-exemple, sans modifier scores ou grille Ensemble pour satisfaire un seuil QA.

### Scénarios discriminants T1–T7

Tous restent GAP pour le delta jusqu'aux preuves. L'auteur écrit les tests ; le lead relit et exécute. Aucun nouvel outil ou processus réel n'est nécessaire.

- **T1→B1/B6/I3.** Deux bundles séquentiels du même plan, descriptions répétées, chemins propres à chacun ; Linux/doubles acceptent le keyword. Ancien double ou TypeError interne : aucun second appel sans keyword, aucune commande après erreur, cause non masquée.
- **T2→B2.** À son entrée, le fake runner lit le payload et vérifie quatre clés/valeurs exactes, hash et inscription déjà publiés, fermeture effective. Contre-exemples : clé superflue, cible dupliquée/mal ordonnée, mauvais root. Une présence constatée seulement après commande est un oracle insuffisant.
- **T3→B2/B3.** Build relit le même fichier et les mêmes octets admis. Mutation/suppression/reçu étranger/incomplet : aucun appel adaptateur/runner après échec d'admission, pas de réparation, pas de succès.
- **T4→B2/B4.** Destinations absentes : créations exclusives aux phases prévues. Fichier/répertoire préexistant ou collision interne, dont `raw/ctest-population.json` : refus avant commande et contenu initial intact. Symlink seulement si fixture supportée ; SKIP explicite sinon, aucun droit ou jonction de substitution.
- **T5→B1/B4.** Répétitions descriptions/CLI list/plan déterministes et arbre inchangé ; prepare autorisé ne lance que prepare. Sans opt-in, aucun helper/bootstrap/build/run ; sentinelles écriture/enfant/réseau causales, jamais fallback réel.
- **T6→B2/B5/I2.** Prepare/build factices réussis attribuent capture commune et logs privés. Interruption aux frontières de publication ou fake enfant nonzero : reçu non favorable, codes/flags/bruts/préfixes disponibles conservés ; nouvelle tentative dans nouveau bundle.
- **T7→B6.** Aller-retour des modèles existants, adaptations de consommateurs après entrée core intégrée. Audit de diff et contrats : aucun champ public, schéma copié, setter/global ou signature build supplémentaire.

Passe heuristique bornée : prepare/build admis/refusé/interrompu ; absent/existant/partiel ; fermeture→empreinte→publication→commande ; descriptions répétables, phases non réparatrices ; concurrence limitée au lease existant.

### Registre obligations↔preuves et orphelins

- B1 : `adapters/base.py::PlatformAdapter.describe_preparation`, `evidence.prepare`→T1/T5.
- B2 : `BundleSink.artifact`, `_build_root`, `prepare`→T2/T4/T6.
- B3 : `_predecessor`, `build`→T3.
- B4 : `verify.main`, `BundleSink.lease`→T4/T5.
- B5 : `_cause`, `_finish`, `prepare/build`→T6.
- B6 : protocole adaptateur, `runtime_fixtures.AdapterDouble`, modèles→T1/T7.

Sens inverse/réemploi : la régression de prédécesseur invalide avant enfant/adaptateur soutient B3 ; les tests help/list/plan et CLI cold sans effets soutiennent B4 ; conservation des logs partiels prepare/build soutient B5 ; `test_plan_contract.AdapterInterfaceTests` soutient B1/B6 et nécessite un mapping borné de signature. Références statiques du planner, pas audit complet des corps ni exécutions du delta. Chaque nouveau test pointe B1–B6 ; aucun test orphelin établi, aucune obligation d'ajouter un doublon faible.

### Charge, preuves, sortie et décisions lead

Sur100unités relatives : setup25 ; planifié56,25 (T1/T3:16,T2/T4:17,T5:8,T6:10,T7/revue:5,25) ; buffer18,75 investigation/panne/ré-audit. Soit75%planifié/25%buffer après setup, sans engagement horaire. Réduire cosmétique/extensions avant contrôles des risques≥10.

Chaque RED/GREEN/non-régression retenu conserve HEAD/delta/empreintes, date/source, commande/cwd/interpréteur, auteur/proposition/exécutant, attendu/observé et nombres découverts/exécutés/échecs/erreurs/skips. Les fondations inchangées gardent leurs preuves attribuées, mais aucun groupe T1–T7 n'est actuellement COVERED.

Sortie : cet addendum commité avant autorisation d'écriture ; RED causal puis GREEN minimal et non-régression par lead ; core réellement intégré avant adaptateurs ; revue indépendante fresh ciblée ownership/publication/pureté puis corrections/ré-audit ; aucun schéma public imprévu, réparation silencieuse ou brut perdu. Zéro test et skip requis ne valent pas PASS.

Le lead accepte le complément et la rectification du lien manquant. Qualification Windows/Linux réelle, C++/OpenGL et performances restent DEFER, propriétaire lead/futur responsable de qualification, à établir avant toute revendication native. Pas de nouveau choix d'architecture ni autorisation de campagne. Le futur auteur ne reçoit que le delta core et son mapping explicite de doubles, pas une permission générale sur les adaptateurs ou les scripts moteur.

## 12. Addendum ratifié — environnement enfant matériel

Proposition qa-planner/pool-21 `result_mvumebdm_0000_uv51g1`, réponse finale
publique complète reçue nativement. Lectures locales du contrat, du runner et du
test de transport réussies ; aucune édition, exécution ou refus de lecture.
Commit contrat fourni au lecteur : `7a5ed1632278add245a6d65b7f86875b60907b36`.
Le lead persiste et ratifie ce plan avant nouveau worktree auteur. Le §11 reste acquis.

### Obligations et risque

Explicites : H1 déclenchement uniquement par delta exactement hardware, sans
dépendance à argv ; H2 six champs explicites non vides, sinon refus avant factory,
sans refaire le validateur sémantique core ; H3 retirer les deux variables
logicielles de la copie et refuser toute présence dans le delta, même vide ou0 ;
H4 contexte hardware seulement ambiant inactif, six contextes ambiants nettoyés,
CPU/sans déclaration et logiciel explicite inchangés ; H5 parent intact,
indépendance des appels, refus comme panne pipeline et non précondition GPU ;
H6 aucun schéma modifié, capture/causes/gestion primaire conservées.

Inférés : I1 comparaison complète du parent avant/après ; I2 factory strict et
compteur des invocations inattendues ; I3 ordre permuté sur la même instance.

Top risques impact×probabilité :
- R1 forçage logiciel conservé,5×4=20 : parent avec une ou deux variables ; portée
  workloads et CTest matériels. Détection : finding statique indépendant ;
  mitigation existante par garde GL ultérieure, insuffisante pour assainir ;
  gap environnement effectif à la factory.
- R2 transport incomplet/réintroduction,4×3=12 : champ absent/vide ou variable
  interdite dans delta ; portée admission et attribution. Détection contrat,
  garde runner absente ; mitigation admission core ; gap refus causal/zéro lancement.
- R3 contamination parent/profils suivants,4×3=12 : nettoyage global ou état
  réemployé ; portée parent/CPU/logiciel. Détection test existant partiel,
  mitigation copie par appel ; gap séquences et comparaison complète après refus.

### Tests, registre bidirectionnel et oracles

Réutiliser les doubles du fichier de transport existant, pas de nouveau harness.
- HT1→H1/H3 : matériel complet avec chaque variable puis paire dans parent,
  argv workload/CTest/découverte. Factory reçoit deux clés absentes, six valeurs
  exactes et marqueur ordinaire conservé ; ni None ni vide de substitution.
- HT2→H2/H5 : chaque champ retiré puis vidé ; présence dans parent ne répare pas
  delta. Refus causal et factory zéro. Le champ sélecteur retiré/vide ne déclenche
  toutefois pas hardware selon H1 : tester ce cas selon HT4, sans contradiction
  H1/H2. Les cinq autres champs incomplets sous sélecteur hardware sont refusés.
- HT3→H3/H5 : chaque clé interdite présente avec1,0 ou vide : refus avant factory.
- HT4→H1/H4 : hardware seulement ambiant nettoyé sans assainissement des deux
  variables ; CPU/sans déclaration garde héritage et logiciel ses valeurs
  explicites. Sélecteur absent/vide/autre non exactement hardware ne déclenche
  ni assainissement ni rejet sémantique nouveau.
- HT5→H4/H5/I1/I3 : même runner CPU→matériel→logiciel puis inverse ; insérer refus
  HT2/HT3, vérifier parent identique et appel suivant non contaminé.
- HT6→H5/H6 : sorties synthétiques succès/nonzero et outcome/capture conservés ;
  refus propagé comme panne, pas code enfant favorable. Réutiliser contrôles
  existants de causes/capture, documenter convention d'exception avant RED sans
  nouvelle classe ou code public imposé par le planner.

Sens obligations→preuves : H1–H3 dans construction d'environnement de
SubprocessRunner.run→HT1–HT3 ; H4/H5 copie/nettoyage par invocation→HT4/HT5 ;
H5/H6 propagation/capture→HT6. Sens inverse : tous HT1–HT6 sont rattachés,
aucun test driver/performance/GPU. Test existant de contexte hérité soutient
H4/H5 statiquement, pas H2/H3. H1–H6 restent GAP jusqu'aux preuves ; aucun COVERED.

### Charge, sortie et arbitrage lead

Base relative100 : setup25 ; planifié56,25 (HT1/3:18,HT2:12,HT4/5:16,
HT6/revue:10,25), buffer18,75. Après setup75%planifié/25%buffer, sans durée promise.
Priorité aux refus et environnement effectif, sans nouvelle infrastructure.

Sortie : plan commité avant auteur unique runner/tests, tests RED avant
implémentation, RED lead puis GREEN/non-régression ciblée, factory factice stricte
sans aucun enfant réel dans cette tranche ; revue indépendante des conditions,
ordre de nettoyage et refus, corrections/ré-audit avant intégration puis
revalidation Linux synthétique. Preuves : candidat/diff/SHA256, source/date,
commande/cwd/interpréteur, auteur/runner, attendu/observé et nombres ; changement
de candidat exige requalification, zéro test ou skip requis ne vaut pas PASS.

Arbitrage lead : complément accepté avec précision HT2 nécessaire pour préserver
H1 ; sélecteur absent ne peut à la fois ne pas déclencher et exiger hardware.
Autres mécanismes externes de sélection du driver hors correction. Environnement
assaini ne prouve pas renderer matériel ; GL/cible exacte réels NON_EXECUTE.

## 13. Addendum ratifié — helper Windows et parents CMake

Proposition QA `result_mvuo139f_0002_rccy4h`, pool-21 ; réponse finale complète
native reçue. Lectures helper, fixtures, CMake et contrats réussies ; aucune
édition ni exécution. HEAD fourni `a7aacb9bb10346c28a637d8264aa539a12f3d733`,
pas de vérification Git du planner. Les §11–12 restent acquis. Toutes obligations
nouvelles ci-dessous sont GAP jusqu'aux preuves, pas COVERED.

### A — Helper prepare/build seulement

Obligations explicites : A1 admission prepare/build uniquement ; run, flags
incompatibles, payload invalide ou prepare sans opt-in refusés avant bootstrap,
environnement ou outils. A2 payload privé quatre champs lié au bundle admis,
jamais recréé/réparé. A3 prepare : Conan2 puis configure, configuration/arbre/cibles
et PRE_TEST cohérents, aucun build/test/workload. A4 build : seules cibles choisies
dans arbre préparé, aucun bootstrap/Conan/configure/réparation. A5 environnement
outil seulement aux commandes possédées, appelant restauré sur succès/nonzero/
exception. A6 legacy, run direct et capture primaire core préservés, aucun
helper-run commun ou nouveau schéma. A7 régions natives pinées inchangées par
défaut, aucun repin, seuls fragments explicitement extraits/factices relus exécutés.

Inférés : sentinelle de fallthrough legacy, événements ordonnés, mutations ciblées
et comparaison parent synthétique, moyens de preuve et non nouvelles fonctions.

Risques impact×probabilité :
- RA1 ouverture trop large5×4=20 : retrait de garde/phase run/flags ; portée outils
  et phases interdites ; détection oracle fermeture actuel, mitigation helper fermé,
  gap admission fine avant effets.
- RA2 fixture masquant binding4×4=16 : root/tooling injectés dans test ou payload
  remplacé par variables ; portée arbre/commandes/attribution ; détection dispatch
  testé avec variables injectées, mitigation wrapper valide, gap chaîne source
  réelle garde→binding→dispatch sans initialisation cachée.
- RA3 erreurs/env/bruts perdus4×3=12 : nonzero/exception/restauration partielle ;
  portée commandes suivantes/preuves ; détection tests existants partiels,
  mitigation finally dispatch ; gap nouvelle composition/première cause.

Mapping préalable obligatoire de
`test_wrappers.WindowsRealHelperBarrierTests.test_real_helper_remains_closed_before_dispatch` :
ancienne fermeture globale→admission prepare/build après validation ; absence
d'effets avant refus→journal vide A-T1 ; absence run/fallback→A-T1 et sentinelle
legacy ; helper complet jamais exécuté→lanceur/extraction bornés A-T0. L'ancien
oracle n'est modifié qu'après adoption de ce mapping détaillé et RED ; jamais
simplement supprimé pour verdir.

Groupes/scénarios et registre bidirectionnel :
- A-T0→A7 : fragments source réels garde/binding/dispatch, marqueurs uniques,
  syntaxe valide et feuilles factices admises. Extraction inattendue/pin modifié/
  appel non autorisé→HARNESS/INCONCLUSIVE, pas RED comportemental.
- A-T1→A1/A6 : prepare autorisé/build valide atteignent dispatch ; opt-in absent,
  run/unsupported/flags incompatibles/payload absent ou invalide→refus causal et
  effets bootstrap/env/outils zéro, aucun legacy fallback.
- A-T2→A2/A3 : vrai payload synthétique quatre champs, Debug/Release et espaces,
  workloads seuls puis tests ; mauvais bundle/arbre/config/champ supplémentaire
  ou commande run transportée→refus avant effets, octets payload intacts.
- A-T3→A3 : journal Conan→CMake, profils host/build et configuration concordants,
  workloads ON ; tests sélectionnés→BUILD_TESTING ON et PRE_TEST, sinon OFF.
  Conan nonzero→pas CMake, ni build/CTest/run/installation hors prepare opt-in.
- A-T4→A4 : commande CMake build unique, arbre/cibles/jobs exacts ; arbre non
  admis/nonzero→aucun bootstrap/configure/réparation, code causal conservé.
- A-T5→A5/A6 : MSVC vu par faux enfant puis restauration exacte ; succès/nonzero/
  exception avec clés initialement absentes/vides ; première cause et bruts conservés.
- A-T6→A6/A7 : revue legacy et non-régression ciblée, aucune chute verification
  vers legacy/helper-run ajouté/région native modifiée.

Sens inverse : oracle fermeture→A-T0/T1 ; test_dispatch/dispatch_fixture→A-T3/T5 ;
Resolve-VerificationInputs/dispatch→A-T2/T3/T4/T5 ; tests adaptateurs payload
ownership/build reuse/common capture→A2/A6 réutilisés sans doublons. Chaque nouveau
test cite ses obligations. Fonctions privées run restent hors preuve run commun.

RED recevable : refus explicite actuel « non implémenté » sur prepare autorisé
avec harness valide. Fonction absente, parse error, PowerShell indisponible,
fixture cassée ou zéro test ne sont pas RED. Le lead relit programme test-owned,
extraction, feuilles, frontière spawn et sorties AVANT exécution. Helper complet
et toute feuille native réelle restent interdits. Pas de discovery Windows globale ;
test06 jonction reste NON_EXECUTE. Limite Dispose/writers hérités privée connue
hors correction, ne devient pas preuve du run commun.

### B — Parents CMake

Explicites : B1 option root ARTIST_BUILD_VERIFICATION OFF par défaut ; B2 inclusion
conditionnelle dans ARTIST/CMakeLists indépendante de BUILD_TESTING et après
disponibilité ArtistLib ; B3 réutiliser deux workloads intégrés, pas custom command/
découverte/lancement ajouté ; B4 Conan2 avant configure, Ninja/config/PRE_TEST
préservés, aucun configure réel dans cette mission. Contrôles inférés : structure
contextualisée, commentaires/chaînes ignorés et contre-exemples synthétiques.

Risques : RB1 activation/exécution implicite5×3=15 (défaut ON/inclusion inconditionnelle,
portée builds ordinaires ; leaf sans lancement, option prévue ; gap garde OFF et
absence effets). RB2 raccordement manquant/dépendant tests4×3=12 (mauvais scope,
portée workloads ; source parents actuellement non raccordés, leaf existant ; gap
matrice logique). RB3 workflow automatique/fallback/cloud5×2=10 : non retiré du
mandat par cette proposition ; voir arbitrage et complément séparé ci-dessous.

B-T1→B1/B2 : matrice OFF/ON×BUILD_TESTING OFF/ON, option ON raccorde indépendamment
tests, OFF exclut. B-T2→B2/B3 : deux cibles exactes atteignables statiquement ;
contre-exemples mauvais chemin, inclusion inconditionnelle, scope tests seul,
lancement ajouté doivent être détectés. B-T3→B3/B4 : diff structurel sans mutation
générateur/toolchain/découverte opportuniste ni commande moteur exécutée. Ces
tests ne prouvent pas configure/compilation réels. Sources/obligations↔preuves :
parents CMake et ARTIST/verification/CMakeLists→B-T1/T2/T3, pas de tests orphelins.

### Charge et sortie

Base relative100 : setup25, planifié56,25 (A admission/binding20, phases/env/causes18,
B structure10, revue/non-régression8,25), buffer18,75 ; après setup75%/25%, sans
engagement horaire. Priorité aux risques≥10, pas nouvelle infrastructure.

Plan commité avant auteurs, ownership distinct helper/tests Windows et parents
CMake/tests intégration ; RED causal puis GREEN/non-régression lead sur fixtures
relues, revue indépendante ciblée A et contrôle garde/raccordement B, corrections/
ré-audit, preuves candidat/delta/hashes/date/commandes/cwd/runner/comptages conservées.
Intégration effective avant consommateurs/docs finales. Zéro test ou skip requis
ne vaut pas PASS. Gap WindowsAdapter→runner→factory effectif reste au generator
ultérieur, pas duplication dans A. Natif/C++/CTest réel/GPU/performance NON_EXECUTE.

Arbitrage lead : A et B source CMake ratifiés. La proposition « pas de nouveau
workflow » n'est PAS retenue : contract.md ligne28 prévoit explicitement
`.github/workflows/verification-pipeline.yml`, confirmé par PM rapport114.
Complément B workflow demandé au planner avec activation manuelle et machines
opérateur préparées ; aucun auteur workflow admis avant ce complément commité.
Cette attente n'empêche pas A, indépendant, de commencer ses tests après ce commit.

### Complément B5/B6 ratifié — workflow manuel obligatoire

Rectification QA `result_mvuo5wsb_0002_hma7aa`, réponse finale complète reçue.
L'omission du workflow est retirée ; `.github/workflows/verification-pipeline.yml`
reste un livrable explicite, pas un GAP différé accepté. Sections A/B précédentes
conservées ; branche B admise seulement après commit de ce complément.

B5 : workflow_dispatch exclusivement, aucun push/PR/schedule/workflow_call ou
déclenchement automatique. Machines opérateur self-hosted Windows/Linux préparées,
pas de hosted/cloud/installation/réparation implicite. Platform/profile/phase
choisis dans ensembles fermés ; vendor/renderer matériels exacts non vides,
aucun label/GPU comme qualification. DISPLAY Linux et MSVC Windows restent
préconditions machine. Workflow jamais invoqué pendant la mission.

B6 : entrées validées avant commande de phase ; chaînes de cible préservées sans
normalisation destructive. Transport environnement/argv délimité, pas interpolation
libre d'expressions d'inputs dans shell. List/plan strictement consultatifs ;
chaque phase explicitement choisie, prédécesseur absent refuse sans cascade
réparatrice prepare/build/run. Identités/journaux/bruts/codes conservés ; artefact
absent, upload réussi ou enfant code0 ne fabriquent pas PASS. Checkout/artefacts
actions à versions explicites selon pratiques locales, aucun budget automatique.

Risques B5 et B6 chacun I5×P3=15. B5 déclencheur trigger automatique/hosted/fallback,
portée lancements non demandés/attribution ; détection contrat/revue seulement,
mitigation manuel+machines préparées, gap structure/chemins de refus. B6 déclencheur
input injecté/cascade/preuves absentes masquées, portée commandes/conclusions ;
détection séparation CLI existante, mitigation choix fermés/transport littéral/
refus prédécesseur, gap composition workflow→argv et conservation après échec.

T4a→B5 : dispatch seul et branches self-hosted bornées ; contre-exemples auto-trigger,
ubuntu-latest/hosted ou choix libre de runner rejetés. T4b→B5/B6 : valeurs admises
littérales et cibles exactes ; inconnues/vides/quotes/retours ligne/métacaractères
refusées ou préservées littéralement, jamais commande supplémentaire. T4c→B6 :
list/plan appellent leur seul mode, phase argv attendus sous doubles ; prédécesseur
absent/installation/cascade refusés sans remplacement. T4d→B6 : codes/artefacts
disponibles conservés succès/échec ; résultat absent/logs partiels/échec puis upload
réussi ne donnent jamais favorable. T4e→B5/B6 : pins actions/préconditions documentés ;
label comme preuve/fallback/install/cloud constituent écart bloquant.

Registre bidirectionnel : workflow↔B5/B6↔T4a–e ; documentation préconditions↔B5/T4e,
chaque nouveau test attribué, état GAP jusqu'aux preuves. Parseur YAML déjà présent
sinon contrôle textuel structuré borné + revue, aucune installation. Doubles argv
sans shell/outils/workflow réels ; vérification textuelle ne prouve pas GitHub Actions.
Choix détaillés auteur proposés avant implémentation ; RED/GREEN lead et revue
indépendante ciblée avant intégration. Aucune extension de droit, budget ou campagne.

## 14. Addendum ratifié — preuves privées et feuilles réelles du helper

Proposition complète `result_mvuq7fp5_0000_3vnwya`, pool-21, lectures réussies,
aucun refus, édition, test ou processus. Contrat `windows-helper-evidence-v1.md`
au commit fourni `b8d036a72d9ebfb3f972023c12c7c36e1a1e3097`. Les §11–13 restent
acquis. Les sept baselines, RED1 et dix-huit tests haut niveau ne prouvent pas
encore ce delta ; W1–W8 restent GAP avant preuves.

### Obligations, risques et moyens de preuve

Explicites : W1 contexte privé par invocation non réentrant après admission,
retiré finally, map enfant sans mutation OS/cache/global/fallback ; W2 Process
unique, ownership Dispose/streams conservé, budget global restant, aucun succès
sur code absent/timeout/exception/cleanup ; W3 rôles/noms/champs exacts du contrat,
création exclusive, command_line réellement transmise, bruts partiels ; W4 témoin
douze chaînes fermé après prepare réussi, SHA du JSON déclaratif pas des binaires ;
W5 témoin empreinté puis réadmis avant adaptateur/tooling, configuration/arbre et
préconditions CMake/Ninja bornées, aucun bootstrap/configure implicite en build ;
W6 collecte Windows possédé sous lease après succès/nonzero/interruption/exception
avant receipt, témoin/rôles nécessaires au succès ; W7 regular files, liens/reparse/
alias/hardlinks interdits refusés, no-clobber, inconnu signalé sans adoption/suppression,
première cause/priorité conservées ; W8 aucun schéma public/parser capacité/deuxième
capture workload, wrappers/primitives pinés intangibles.

Inférés : sentinelles d'effets, compteur factory, horloge factice, parent synthétique
comparé et mutants ciblés, pas de nouvelles fonctionnalités.

Top risques impact×probabilité : R1 receipt favorable sans preuve5×4=20 ; déclencheur
zéro sans témoin/rôle ou témoin modifié/étranger, portée attribution prepare/build,
détection payload/empreintes génériques, mitigation receipts candidat, gap validation/
collecte/réadmission privées. R2 adoption/perte de partiels5×3=15 ; alias/ancien
dossier/inconnu/exception collecte, portée intégrité/diagnostic, détection confinement
existant, mitigation lease/chemins bornés, gap ownership/population et causes secondaires.
R3 doubles masquant wiring/budget/cleanup4×4=16 ; child remplacé, timeout réinitialisé,
map persistante ou Dispose échoué, portée commandes/verdicts, détection haut niveau
et primitives, mitigation factory/contexte, gap nouvelles coutures et exporter réel borné.

### Branche core — C1–C6 sans processus

- C1→W3/W4/W6 : faux prepare/build publient données conformes et hash avant receipt ;
  enfant0 sans témoin/rôle nécessaire reste défavorable. Toutes les traces présentes : code0/flags faux/error null
  requis pour succès, pas simple présence d'un fichier.
- C2→W4/W5 : témoin valide lié au plan/arbre, octets réadmis ; absence/mutation/
  étranger/clé inconnue ou manquante/doublon JSON/type ou format invalide refuse
  avant adaptateur/runner, sans rewrite.
- C3→W3/W6/W7 : collecte sur tous retours et interruptions/exceptions ; bruts partiels
  ou trace incomplète conservés, anomalie visible, aucun témoin de succès inventé.
- C4→W3/W7 : population fermée, transports allowlistés restants collectés en échec ;
  rôle/fichier/dossier inconnu ou collision ancien→défavorable, inconnu préservé
  non adopté, pas parcours récursif arbitraire.
- C5→W7 : fichiers réguliers distincts ; liens/reparse/hardlink/alias refusés sans
  clobber. Précondition indisponible→limite/skip explicite, pas privilège ni PASS.
- C6→W6/W7/W8 : doubles génériques sans transport inchangés ; helper échoué puis
  collecte anormale conserve première cause/secondaire et priorité existante.

Mapping préalable ratifié, assertions métier conservées :
- `test_bundle_preparation_contract.py::OwnedBundleRunner.run` → contrats B1–B6
  payload fermé/hash avant commande, séparation/permissions/codes → succès Windows
  bootstrap manque journaux/témoin → ajouter ces seules données privées conformes
  sous destinations possédées, pas en Linux/générique ni avant refus/interruptions.
- `verification_windows/binding_fixtures.py::BindingRunner.run` → vrais reçus seed
  et composition de l'adaptateur, mapping/cache/argv/capture → succès prepare/build
  manque témoin/rôles → ajouter données synthétiques conformes, garder les fakes
  inertes et leurs compteurs/outcomes/codes. Aucun moteur ni CTest émulé en plus.
- Un helper de données commun dédié aux tests core est permis pour cette seule
  production factice ; ne pas importer la validation production pour fabriquer
  l'oracle. Aucun changement des assertions des 22/54 tests ni des génériques.

Tout autre fake touché nécessite un mapping complémentaire avant édition. Les
nouveaux tests sont proposés et exécutés en RED avant ces adaptations ou le core.

### Branche PowerShell — P0–P5

Nouvelle fixture test-owned, data/factory mémoire, source réelle de wiring/exporter
non remplacée entièrement, seuls effets doubles. P0→W8 baseline syntaxe/extraction/
pins/factory avant RED ; fonction absente, pin inattendu ou harnais cassé reste
HARNESS/INCONCLUSIVE. P1→W1 map copiée null/vide, root source réel, contexte absent/
réentrant refusé, invocations indépendantes, parent OS intact. P2→W2/W3 factory
observe chemin/cwd absolus, command line exacte et Process unique ; absent/nonzero/
exception défavorables, Dispose/cause/sorties conservés. P3→W2 horloge factice
budget restant, épuisement avant commande suivante→zéro lancement, pas reset,
cleanup échoué visible. P4→W3 exporter source borné et journaux/transports exacts
exclusifs ; collision/inconnu/cleanup sans clobber ou suppression étrangère, partiels
conservés. P5→W4/W5 témoin après succès seul puis build sans rewrite ; VS explicite
invalide, témoin/cache/toolchain/build.ninja divergent/absent refuse avant effets.

Revue lead obligatoire de toute nouvelle source/fixture et pin privé avant run ;
aucun repin WRAPPERS/NATIVE. Le gap Dispose/writers hérités reste ouvert. Toute
adaptation de métadonnée fake VSINSTALLDIR doit être annoncée avant changement,
sans altérer les assertions de path/configuration ou injecter root dans l'entrée.

### Registre, charge et sortie

W1/W2↔contexte/factory/binding↔P1–P3 ; W3/W4↔exporter/témoin/collecteur↔P4/P5/C1–C4 ;
W5↔_admit_windows_helper/build/admission PS↔C2/P5 ; W6/W7↔collecte prepare/build avant
_finish↔C1/C3–C6 ; W8↔diff/pins/génériques↔P0/C6. Chaque nouveau test cite ces
obligations ; aucun orphelin établi. Les dix-huit haut niveau restent utiles,
sans remplacer les nouveaux oracles de feuilles.

Après les deux intégrations seulement : couture synthétique témoin réellement
empreinté dans PreparationReceipt.artifacts puis réadmis ; mutation refusée avant
adaptateur/tooling. Pas de consommation implicite d'un diff de branche.

Base100 : setup25, planifié56,25 (core24,PS24,couture/revue8,25), buffer18,75 ;
75%/25% après setup, sans durée promise ni outil supplémentaire. Plan/mappings
commités avant auteurs, ownership core/helper distinct, baselines valides puis
RED/GREEN lead et non-régression, collecte/no-clobber/causes/réadmission démontrés,
revue indépendante puis couture intégrée. Preuves candidat/diff/hashes/date/
commandes/cwd/runner/attendu-observé/comptages conservées ; candidat changé exige
requalification, zéro test/skip requis ≠ PASS. Natif et identité binaire outils
non démontrés ; aucun workflow, CMake réel, moteur, GPU, installation ou cloud.

## 15. Compléments finaux G1–G4 — plan generator ratifié

Proposition textuelle complète `result_mvuxb9xc_008c_y6ti6z`, test-engineer/pool-37,
reçue après restitution complète demandée. Lectures et profil restrictif vérifiés
au rapport143 ; aucun code, patch, test ou édition produit par ce profil. Plan PM
`ses_e734d125dffejyP9S8sAZXcEFw`, lecture seule, validé par le lead. Entrées réelles :
helper52e7330, collecteur9aedc0a et vérifications rapport142, guide9a45bf6. Les
résultats existants ne couvrent pas ces compléments ; G1–G4 restent INCONCLUSIVE.

### Obligations et risques

- G1/W3–W6, I5×P4=20 : reçu favorable sans continuité helper→collecte→réadmission.
  Déclencheur : témoin préfabriqué ou copié d'un autre bundle ; portée attribution
  prepare/build. Détection : octets/hash/ordre/lease, mitigation fixtures et reçus
  existants, gap couture réelle sur les mêmes artefacts.
- G2, I5×P3=15 : delta décrit correctement mais environnement effectif erroné.
  Déclencheur : contexte ambiant ou forçage logiciel hérité ; portée workloads et
  CTest Windows. Détection factory.env effective ; mitigation runner assaini,
  gap composition du véritable adaptateur avec ce runner.
- G3, I5×P3=15 : suppression d'une ancienne barrière sans remplacement des garanties.
  Portée admission/zéro effet/fallback ; détection structure et tests fonctionnels,
  mitigation mapping préalable, gap ancien oracle obsolète.
- G4, I4×P4=16 : prototype dispatch obsolète pris pour preuve actuelle ou imposant
  helper-run. Portée sélection/codes/argv/restauration ; détection inventaire
  bidirectionnel, mitigation tests actuels, gap migration visible des anciens tests.

### G1 — couture du même bundle et témoin

Nominal : fragments PowerShell réels admis sous factory mémoire, appelés pendant
evidence.prepare dans le bundle du vrai BundleSink. Rôles et témoin proviennent
de la source, jamais de witness_data ou d'un faux reçu. Avant _finish : leases
bundle/arbre actives, writers fermés et SHA256 calculés indépendamment égaux au
manifeste et PreparationReceipt. Build utilise ce reçu et les mêmes octets,
sans réécriture, bootstrap ou configure ; une capture primaire commune par commande,
pas deuxième exécution de preuve.

Négatifs minimaux : mutation ou suppression du témoin après prepare favorable,
bundle distinct par cas ; refus avant description build/runner/tooling, sans
remplacement. Un échec Conan après partiels : pas de configure ou témoin favorable,
partiels collectés avant reçu et première cause conservée. Ne pas dupliquer la
matrice exhaustive du schéma déjà couverte dans le cœur.

KEEP cible, DEFER **seule réalisation** jusqu'à validation du plan de fixture A0,
pas abandon du critère. NativeBindingCase.run_case recrée son bundle ; HelperRunner
fabrique ses données. Leur juxtaposition ne suffit pas. Symboles vérifiés :
runtime_fixtures.RuntimeFixture, BundleSink, evidence.prepare/build,
NativeBindingCase/fragments, native_binding_fixture.ps1 et ArtistBindingMemoryProcess.
Le nom SINKFixtureRuntime évoqué dans un brief n'existe pas : aucune API supposée.

A0 propose avant code une adaptation test-owned minimale vers le bundle unique,
ses sentinelles et ses extractions. Installer les programmes source nécessaires
avant snapshot, ne pas masquer les contrôles d'identité. A1 n'écrit que de nouveaux
tests/fixtures dédiés, noms réservés à l'acceptation A0 ; aucun changement de
production ou fixture historique commune.

### G2 — environnement effectif Windows

KEEP nouveau test test_effective_windows_environment.py. Réutiliser vrais
WindowsAdapter, plans/reçus existants et SubprocessRunner(popen_factory mémoire).
Pour chaque route workload, découverte et exécution CTest : parent entièrement
synthétique, variables de contexte périmées et chacun/pair des deux forçages.
Observer à la factory argv/cwd exacts, shell=False/stdin DEVNULL/captures possédées,
six champs attendus dérivés du plan/reçu (pas tous recopiés d'env_delta), identifiants
liés, absence des deux forçages, un Process mémoire et parent intact.

CPU : six contextes ambiants retirés, marqueurs et politique d'héritage des forçages
inchangés, parent préservé. KEEP refus Windows opengl-software déjà existant ;
REJECT un test d'exécution logicielle Windows copié depuis Linux. Les preuves de
codes/capture du runner sont réutilisées, pas dupliquées sans besoin.

### G3 — remplacement ponctuel de la fermeture globale

MODIFY test_wrappers.WindowsRealHelperBarrierTests.test_real_helper_remains_closed_before_dispatch.
Conserver la classe et autres assertions ; nom et oracle deviennent admission
fine prepare/build. Vérifier unicité/ordre des régions et root→flags→refus run→
payload→inputs→admission arbre build, absence d'effets avant dispatch, legacy et
pins intangibles. Proposition SOURCE_GREEN_READY.md136–163 comme base à relire.
Structure seule ≠ preuve fonctionnelle : conserver invalid phases/opt-in/flags,
payload/binding zéro effets, mutations/extraction et sentinelle des suites actuelles.

### G4 — mapping bidirectionnel du prototype dispatch

MODIFY/REPLACE la couverture de test_dispatch.py et dispatch_fixture.ps1, pas
réintroduire helper-run pour leurs anciennes attentes. REPLACE signifie migration
visible des obligations et de leur inventaire, pas suppression d'une garantie.

Ancien WindowsDispatchTests → preuves conservées :
- prepare_without_opt_in_refuses_before_effects → admission fonctionnelle phases/
  opt-in/flags, journal zéro effets.
- prepare_authorized_runs_conan_then_cmake_only → prepare_conan_before_fresh_cmake_profiles_trees_and_testing.
- build_only_selected_target_without_bootstrap_or_configure → build_is_one_selected_command,
  refus arbre/tooling.
- run_only_preserves_argument_boundaries → adapter05/06/19, G2 argv factory et
  wrappers quoting ; REJECT helper-run.
- child_failure_preserves_raw_code_and_stops_sequence → first_nonzero_stops_prepare_and_preserves_raw_data,
  native first-cause.
- environment_present_and_absent_restored_after_success/failure → functional
  child_environment_and_restore_success_nonzero_exception_absent_empty, G2 parent.
- selected_phases_never_fall_through_to_legacy_sentinel → admission et sentinelle,
  phases prepare/build admises, run refusé.
- fixture_refuses_unapproved_effects_before_evaluation → mutations/extraction
  functional et contrôles source natifs.
- fixture_can_record_fake_child_and_restore_map_on_exit → baselines functional/
  exception et factory native0/17.

Mapping inverse : admission functional+G3→admission/zéro effet/fallback ; prepare/
build functional→ordre/cibles/stop ; adapter+G2→run direct argv/env ; wrappers/native→
quoting Windows/command line transmise ; harness functional/native→mutations/pins ;
core/native+G1→témoin/collecte/receipt/réadmission. Conserver les tests wrapper
quote_round_trip_through_owned_finite_python_child et child_wrapper_preserves_code_17_and_all_arguments,
ainsi que native real_child_wrapper_to_real_binding_records_exact_start_info.
REJECT skip global/archive invisible ou baisse d'assertions pour masquer la migration.

### Graphe, ownership et sortie

H0 : présent plan commité avant worktrees, HEAD/diff fournis par le lead.
A0 recherche/plan de couture, pas code (PM/LLM63/59) ; A1 attend son acceptation
persistée, tests uniquement (78/73, complexité brute78, plancher70 respecté).
B G2+G3/G4, tranche cohérente (73/66, complexité brute73) : nouveaux tests effectifs,
test_wrappers.py, test_dispatch.py, dispatch_fixture.ps1 ; mapping détaillé avant
édition, aucun fichier A ni production/pin modifié. Affinité build-engineer pour
les scripts/harness, pas substitution du profil QA restrictif par un runner.

A0/A1 et B indépendants en lecture/écriture ; exécutions PS/bundles sérialisées
par le coordinateur. Consommateurs retenus dans le plan : revue des tests et audit
final test-reviewer fresh hors board d'implémentation, après réelle intégration des
deux branches (estimation lecture61/60). Aucun fin auteur ne libère ces consommateurs.

Sur production intégrée correcte, caractérisation et contre-exemples discriminants
sans faux RED historique ; bug production révélé reste rouge et retourne au
propriétaire. Aucun processus auteur. Lead relit programmes/patches, persiste et
exécute sous mandat runner distinct, conserve candidat/hash/commandes/cwd/comptages/
skips/causes, puis vérifie diff intégré. Les grants d'édition/exécution autonome QA
restent absents. Budget proportionné : après setup75% planifié/25% buffer, pas de
campagne ou infrastructure nouvelle pour ces gaps. Encodage cmd non ASCII,
Dispose/writers hérités, jonction06 et cinq skips connus restent non qualifiés.
