# 96 — Raccordements Windows et Linux admis en parallèle

Les interfaces communes, leur QA et leur revue sont intégrées au HEAD `8c7e51b28ae085ada2eb98c97b292fa7147e2988`. Les deux branches de plateforme sont maintenant admises, en contexte fresh et worktrees distincts, sur le profil build-engineer effectivement confirmé par le statut natif. Elles commencent par plan borné et tests RED, sans exécution par leurs auteurs.

- **Raccordement Windows**, tâche `task_mvukzhsc_0052_f1bbfq`, pool-23 : `adapters/windows.py`, helper Windows et nouveaux tests dédiés. Évaluation PM/LLM reprise du plan initial : complexité brute69, retenue70, risque58. Les 51 contrôles de fondation sont conservés ; mapping préalable requis avant adaptation des attentes de fermeture du helper.
- **Raccordement Linux**, tâche `task_mvul0bd4_0053_ptc60a`, pool-24 : `adapters/linux.py`, helper Linux et nouveaux tests dédiés. Évaluation PM/LLM initiale78/71. Release est déjà intégré ; tests6+19 conservés. Contrôleur historique et presets restent inchangés sauf besoin proposé avant édition. Le brouillon non qualifié de pool-19 n'est pas copié.

Les deux tâches dépendent des vrais producteurs terminés : cœur `task_mvu0yczu_0005_li9i9z`, complément QA `task_mvujeqpu_004m_3sfeit`, revue du delta `task_mvukhkg1_004x_k5hgi3`. Les fondations de plateforme déjà reçues sont attribuées aux rapports81/82 ; les tâches parent Windows/Linux restent des suivis de livraison globale, pas des permissions de lancer deux auteurs sur les mêmes fichiers. L'ownership restant est explicitement transféré aux nouveaux auteurs seuls actifs.

Le parallélisme est réel : scripts, modules et fixtures de plateformes distincts ; aucun fichier core/C++/workflow partagé n'est mutable par ces auteurs. Le coordinateur sérialisera les exécutions de tests et reste leur seul runner. Une bibliothèque commune manquante ou une frontière durable non fixée revient au lead, sans sous-délégation ni copie de schéma.

Les critères restent run direct avec exécutables du BuildReceipt admis et arguments visibles, prepare/build séparés réutilisant les helpers, profils CPU/logiciel/matériel explicites, aucun fallback ni nouveau backend. Toute commande réelle MSVC/vcvars, Conan/CMake/CTest, Xvfb, moteur ou GL reste interdite pendant cette mission. Les essais futurs doivent utiliser uniquement des doubles et programmes factices préalablement relus.

Les consommateurs d'intégration CMake/CI/documentation et la QA finale restent différés jusqu'aux adaptateurs réellement intégrés. Aucun nouveau test, moteur, compilation, rendu, benchmark, installation ou cloud exécuté pour cette admission ; aucun modèle explicite ou changement de politique.
