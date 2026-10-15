# ADR-0003 — état et limites Shader→Pass (2026-10-03)

Autorité : docs/adr/0003-ownership-et-lifetime-cpu-gpu.md, Accepted/partial. Source complète : docs/references/session-2026-10-03-shader-pass.md. Mémoire de reprise, pas certificat de correctness.

Implémenté : IShader possède ShaderContext en unique_ptr ; Shader owns GLuint shaderID. Pass owns native program ; PassContext owns vector<unique_ptr<IShader>>, nullptr rejeté, emprunts ref. Shader/Pass non copiables, move-constructibles ; Pass::shared supprimé, withUniform -> IPass&. Composants OpenGL shader/pass on(Context&), HasBorrowedOnMethod seulement ces concepts. Uniform/Attribute/Pipeline partagés hors tranche. Attention IPass ctor unique_ptr MAIS m_context et getContext encore shared_ptr, durée du contexte/shaders extensible hors wrapper.

Destructeurs dérivés ~Shader/~Pass libèrent ; bases ne dispatchent pas dans opération pure. Freer idempotent remet ID=0 ; IDs ont NSDMI =0 ; échec link deleteProgram et ID=0. Pass use/withUniform/getShader/getUniforms/getAttributes/getShadersCount utilisent garde ctx() -> ERROR::PASS::FREED_CONTEXT ; getContext retourne un shared_ptr vide sur moved-from (pas throwing). Shader getContext/load gardés, free moved-from no-op. Contexte/thread GL valide reste précondition du consommateur.

Contrat strict B retenu sur avis engine-architect : loaders rejettent handle !=0 avant leur glCreate*, ERROR::{SHADER,PASS}::RELOAD_WITHOUT_FREE. Pas de remplacement/rollback implicite. Shader recompile = free(); load(). Pour pass complet : libérer programme ET tous shaders, puis pass.load. La valeur du handle est runtime pendant préparation, pas compile-time. IPass::load charge shaders AVANT programme : rejet shader peut précéder rejet pass et non-mutation de la passe entière NON prouvée. Deux tests mock non-mutation + un test shader driver ajoutés.

Revue correctness-reviewer ses_efdc83c33ffektX1dnENb8c1Db : PASS lecture avant F1/F2/F3, 0 blocker selon reviewer, mais 3 should-fix. Coordinateur a corrigé puis rejoué suites finales 122/122. Ce n’est pas une revue indépendante du final ni une absence d’UB exhaustive.

Risque prioritaire encore ouvert : IShader/IPass move-assignment remplacent m_context sans free natif destination ; opérateurs dérivés defaulted. Move-constructor testé != assignation vers owner chargé. Expérience : charger source/destination, assigner, vérifier old destination deleted une fois et transferred handle détruit une fois (mock + driver). Ne pas déclarer tous les moves sûrs.

Orientations architecte ses_efdbcae34ffeXkOhCExMByCiIT (non implémentées) : commencer PipelineContext::m_passes -> vector<unique_ptr<IPass>>, forPass -> IPass&, puis IPass context unique ; durcir UniformReader absent OpenGLPassFlow dans tranche uniform/attribute via test négatif concept. Pas adoption implicite ADR-0002/0004. Autres suivis : `mem:engineering/incidents-2026-10-03`.
