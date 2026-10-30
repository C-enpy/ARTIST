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
