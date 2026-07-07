// VampireSurvivors.Runtime, Version=0.0.0.0, Culture=neutral, PublicKeyToken=null
// VampireSurvivors.Objects.Projectiles.WhipProjectile
using System;
using DarkTonic.MasterAudio;
using Unity.Profiling;
using UnityEngine;
using UnityEngine.Bindings;
using VampireSurvivors.App.Tools;
using VampireSurvivors.Data;
using VampireSurvivors.Framework;
using VampireSurvivors.Framework.PhaserTweens;
using VampireSurvivors.Objects.Characters;
using VampireSurvivors.Objects.Pools;
using VampireSurvivors.Objects.Projectiles;
using VampireSurvivors.Objects.Weapons;

public class WhipProjectile : Projectile
{
	private MultiTargetTween _scaleTween;

	private MultiTargetTween _alphaTween;

	public unsafe override void InitProjectile(BulletPool pool, Weapon weapon, int index)
	{
		//IL_007f: Expected I, but got O
		//IL_0044: Unknown result type (might be due to invalid IL or missing references)
		//IL_0049: Expected O, but got Unknown
		//IL_0100: Expected F4, but got I4
		//IL_011a: Expected O, but got I4
		//IL_012b: Unknown result type (might be due to invalid IL or missing references)
		//IL_0130: Expected O, but got Unknown
		//IL_0147: Unknown result type (might be due to invalid IL or missing references)
		//IL_014c: Expected O, but got Unknown
		//IL_00ad: Unknown result type (might be due to invalid IL or missing references)
		//IL_00b2: Expected O, but got Unknown
		//IL_00c9: Unknown result type (might be due to invalid IL or missing references)
		//IL_00ce: Expected O, but got Unknown
		//IL_01e9: Expected I, but got O
		//IL_018c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0191: Expected O, but got Unknown
		//IL_01a8: Unknown result type (might be due to invalid IL or missing references)
		//IL_01ad: Expected O, but got Unknown
		//IL_01ca: Unknown result type (might be due to invalid IL or missing references)
		//IL_01cf: Expected F4, but got Unknown
		//IL_029d: Unknown result type (might be due to invalid IL or missing references)
		//IL_02a2: Expected F4, but got Unknown
		//IL_021c: Unknown result type (might be due to invalid IL or missing references)
		//IL_0221: Expected O, but got Unknown
		//IL_0238: Unknown result type (might be due to invalid IL or missing references)
		//IL_023d: Expected F4, but got Unknown
		//IL_033e: Expected O, but got I4
		//IL_02f7: Expected O, but got I4
		//IL_046b: Expected O, but got I4
		//IL_043c: Expected O, but got I4
		//IL_0445: Expected O, but got I4
		//IL_04d7: Expected I4, but got I8
		//IL_04e0: Expected O, but got I4
		//IL_0573: Expected O, but got I4
		//IL_035d: Expected O, but got I4
		//IL_0366: Expected O, but got I4
		//IL_06ce: Expected O, but got I4
		//IL_071d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0722: Expected O, but got Unknown
		//IL_0734: Expected I4, but got O
		//IL_0741: Unknown result type (might be due to invalid IL or missing references)
		//IL_0746: Expected O, but got Unknown
		//IL_0810: Expected I, but got O
		//IL_088e: Expected O, but got I4
		//IL_08bd: Expected O, but got I4
		//IL_08ef: Unknown result type (might be due to invalid IL or missing references)
		//IL_08f4: Expected O, but got Unknown
		//IL_0913: Unknown result type (might be due to invalid IL or missing references)
		//IL_0918: Expected O, but got Unknown
		//IL_09d4: Expected O, but got I4
		//IL_097e: Expected O, but got I4
		//IL_0a9b: Expected I4, but got I8
		//IL_0c9f: Unknown result type (might be due to invalid IL or missing references)
		//IL_0ca4: Expected O, but got Unknown
		//IL_0cb6: Expected I4, but got O
		//IL_0cc3: Unknown result type (might be due to invalid IL or missing references)
		//IL_0cc8: Expected O, but got Unknown
		//IL_0e8b: Expected I, but got O
		//IL_0f5d: Expected O, but got I4
		//IL_0ebf: Unknown result type (might be due to invalid IL or missing references)
		//IL_0ec4: Expected F4, but got Unknown
		//IL_1108: Unknown result type (might be due to invalid IL or missing references)
		//IL_110d: Expected O, but got Unknown
		//IL_111f: Expected I4, but got O
		//IL_112c: Unknown result type (might be due to invalid IL or missing references)
		//IL_1131: Expected O, but got Unknown
		//IL_0e59: Unknown result type (might be due to invalid IL or missing references)
		//IL_0e5e: Expected O, but got Unknown
		//IL_0e7d: Unknown result type (might be due to invalid IL or missing references)
		//IL_0e82: Expected O, but got Unknown
		//IL_1060: Expected O, but got I4
		//IL_126f: Unknown result type (might be due to invalid IL or missing references)
		//IL_1274: Expected F4, but got Unknown
		//IL_1423: Expected O, but got I4
		//IL_1477: Expected I4, but got O
		//IL_14aa: Expected I4, but got O
		//IL_14b3: Expected O, but got I4
		//IL_150b: Expected I4, but got O
		//IL_1514: Expected O, but got I4
		//IL_1569: Expected O, but got I4
		//IL_1576: Expected I, but got O
		//IL_159d: Expected I4, but got O
		//IL_15a6: Expected O, but got I4
		//IL_162f: Expected I, but got O
		//IL_1676: Expected I4, but got O
		//IL_167f: Expected O, but got I4
		//IL_1610: Unknown result type (might be due to invalid IL or missing references)
		//IL_1615: Expected F4, but got Unknown
		//IL_16e5: Expected I4, but got O
		//IL_16ee: Expected O, but got I4
		//IL_1724: Expected I, but got O
		//IL_177a: Expected I4, but got O
		//IL_1783: Expected O, but got I4
		//IL_17dd: Expected O, but got I4
		//IL_17fd: Expected O, but got I4
		//IL_17ef: Expected O, but got I4
		//IL_184f: Expected I4, but got O
		//IL_186b: Expected F4, but got I8
		//IL_187d: Expected O, but got I8
		//IL_188f: Expected F4, but got I8
		//IL_18a8: Expected F4, but got O
		//IL_18be: Expected I, but got O
		//IL_1911: Expected F4, but got O
		//IL_1911: Expected I4, but got F4
		//IL_1911: Expected F4, but got I8
		//IL_1911: Expected O, but got I4
		//IL_1911: Expected I4, but got O
		//IL_01e0->IL01e0: Incompatible stack heights: 6 vs 5
		//IL_0277->IL0277: Incompatible stack heights: 6 vs 5
		//IL_0445->IL0445: Incompatible stack heights: 9 vs 6
		//IL_05fd->IL05fd: Incompatible stack heights: 8 vs 7
		//IL_03ae->IL02d9: Incompatible stack heights: 8 vs 6
		//IL_03ae->IL03ae: Incompatible stack heights: 8 vs 6
		//IL_07a0->IL0552: Incompatible stack heights: 8 vs 7
		//IL_0746->IL0746: Incompatible stack heights: 9 vs 8
		//IL_0876->IL07a0: Incompatible stack heights: 13 vs 8
		//IL_09ae->IL0a19: Incompatible stack types: I4 vs O
		//IL_0880->IL0880: Incompatible stack heights: 13 vs 12
		//IL_0a19->IL0932: Incompatible stack heights: 16 vs 13
		//IL_0a6d->IL0552: Incompatible stack heights: 16 vs 7
		//IL_0ac2->IL0552: Incompatible stack heights: 15 vs 7
		//IL_0b1c->IL0552: Incompatible stack heights: 15 vs 7
		//IL_0c0a->IL0552: Incompatible stack heights: 15 vs 7
		//IL_0bb9->IL03da: Incompatible stack heights: 15 vs 6
		//IL_0c61->IL0cc8: Incompatible stack heights: 16 vs 15
		//IL_0c70->IL0d08: Incompatible stack heights: 16 vs 19
		//IL_0d62->IL0552: Incompatible stack heights: 19 vs 7
		//IL_0fa3->IL0552: Incompatible stack heights: 22 vs 7
		//IL_0ed5->IL0ed5: Incompatible stack heights: 22 vs 21
		//IL_0f19->IL06c0: Incompatible stack heights: 21 vs 8
		//IL_0f4f->IL0f4f: Incompatible stack heights: 22 vs 21
		//IL_0e34->IL0d62: Incompatible stack heights: 22 vs 19
		//IL_0e82->IL0e82: Incompatible stack heights: 22 vs 21
		//IL_11a3->IL064e: Incompatible stack heights: 24 vs 7
		//IL_1009->IL0f19: Incompatible stack heights: 23 vs 21
		//IL_1079->IL0a19: Incompatible stack heights: 25 vs 16
		//IL_12c9->IL0565: Incompatible stack heights: 25 vs 7
		//IL_10c9->IL046b: Incompatible stack heights: 25 vs 8
		//IL_10c9->IL10c9: Incompatible stack heights: 25 vs 22
		//IL_125c->IL11a3: Incompatible stack heights: 26 vs 24
		//IL_139f->IL064e: Incompatible stack heights: 25 vs 7
		//IL_133b->IL04ff: Incompatible stack heights: 25 vs 7
		//IL_1285->IL1285: Incompatible stack heights: 27 vs 25
		//IL_1450->IL064e: Incompatible stack heights: 27 vs 7
		//IL_14e6->IL064e: Incompatible stack heights: 28 vs 7
		//IL_1547->IL064e: Incompatible stack heights: 28 vs 7
		//IL_15d4->IL064e: Incompatible stack heights: 31 vs 7
		//IL_16ac->IL064e: Incompatible stack heights: 31 vs 7
		//IL_1626->IL1626: Incompatible stack heights: 32 vs 31
		//IL_171b->IL064e: Incompatible stack heights: 31 vs 7
		//IL_17b1->IL064e: Incompatible stack heights: 31 vs 7
		bool flag = (long)new string("琀愀爀最攀琀\u2000漀瀀攀爀愀渀搀㨀\u2000") != 0L;
		float num2 = default(float);
		float num = num2;
		object obj = default(object);
		if (!flag)
		{
			Console.WriteLine("Unknown call target operand: 26 Call \"il2cpp_codegen_initialize_runtime_metadata\", v30 @ rax_v1, typeof(VampireSurvivors.Framework.GM), pool @ rdx (VampireSurvivors.Objects.Pools.BulletPool), weapon @ r8 (VampireSurvivors.Objects.Weapons.Weapon), index @ r9 (System.Int32)");
			obj = num2 | 0L;
			Console.WriteLine("Unknown call target operand: 29 Call \"il2cpp_codegen_initialize_runtime_metadata\", v47 @ rax_v2, methodof(System.Nullable`1<System.Single>::.ctor), pool @ rdx (VampireSurvivors.Objects.Pools.BulletPool), weapon @ r8 (VampireSurvivors.Objects.Weapons.Weapon), index @ r9 (System.Int32)");
		}
		Console.WriteLine("Unknown operand: Method: VampireSurvivors.Objects.Projectiles.Projectile::InitProjectile");
		WhipProjectile whipProjectile = default(WhipProjectile);
		((Projectile)whipProjectile).InitProjectile(pool, weapon, index);
		nint num3 = (nint)new RenderingExtensions();
		object obj3 = default(object);
		if (new string("Unmanaged memory load: [v48 @ rcx_v12 (Il2CppClass<VampireSurvivors.App.Tools.RenderingExtensions>)+E4]") == null)
		{
			object obj2 = obj | 0L;
			Console.WriteLine("Unknown call target operand: 32 Call \"il2cpp_codegen_initialize_runtime_metadata\", v63 @ rax_v3, typeof(System.Object[1]), pool @ rdx (VampireSurvivors.Objects.Pools.BulletPool), weapon @ r8 (VampireSurvivors.Objects.Weapons.Weapon), index @ r9 (System.Int32)");
			obj3 = obj2 | 0L;
			Console.WriteLine("Unknown call target operand: 35 Call \"il2cpp_codegen_initialize_runtime_metadata\", v72 @ rax_v4, typeof(VampireSurvivors.App.Tools.RenderingExtensions), pool @ rdx (VampireSurvivors.Objects.Pools.BulletPool), weapon @ r8 (VampireSurvivors.Objects.Weapons.Weapon), index @ r9 (System.Int32)");
			Console.WriteLine("Unknown call target operand: 74 Call \"il2cpp_runtime_class_init_actual\", v64 @ rax_v11, typeof(VampireSurvivors.App.Tools.RenderingExtensions), pool @ rdx (VampireSurvivors.Objects.Pools.BulletPool), weapon @ r8 (VampireSurvivors.Objects.Weapons.Weapon), index @ r9 (System.Int32)");
		}
		new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
		SpriteRenderer spriteRenderer = RenderingExtensions.SetAlpha((SpriteRenderer)(object)new string("Unmanaged memory load: [188A104DC]"), 0f);
		Transform transform = ((Component)0).transform;
		object obj4 = obj3 | 0L;
		Console.WriteLine("Unknown call target operand: 38 Call \"il2cpp_codegen_initialize_runtime_metadata\", v78 @ rax_v5, typeof(VampireSurvivors.Framework.SoundManager+SoundConfig), pool @ rdx (VampireSurvivors.Objects.Pools.BulletPool), weapon @ r8 (VampireSurvivors.Objects.Weapons.Weapon), index @ r9 (System.Int32)");
		object obj5 = obj4 | 0L;
		Console.WriteLine("Unknown call target operand: 41 Call \"il2cpp_codegen_initialize_runtime_metadata\", v93 @ rax_v6, typeof(VampireSurvivors.Framework.SoundManager), pool @ rdx (VampireSurvivors.Objects.Pools.BulletPool), weapon @ r8 (VampireSurvivors.Objects.Weapons.Weapon), index @ r9 (System.Int32)");
		bool flag2 = new string("Unmanaged memory load: [18996AC96]") != null;
		float num4 = num;
		object obj7 = default(object);
		if (!flag2)
		{
			object obj6 = obj5 | 0L;
			Console.WriteLine("Unknown call target operand: 44 Call \"il2cpp_codegen_initialize_runtime_metadata\", v114 @ rax_v7, typeof(DG.Tweening.TweenCallback), pool @ rdx (VampireSurvivors.Objects.Pools.BulletPool), weapon @ r8 (VampireSurvivors.Objects.Weapons.Weapon), index @ r9 (System.Int32)");
			obj7 = obj6 | 0L;
			Console.WriteLine("Unknown call target operand: 47 Call \"il2cpp_codegen_initialize_runtime_metadata\", v133 @ rax_v8, typeof(VampireSurvivors.Framework.PhaserTweens.TweenConfig), pool @ rdx (VampireSurvivors.Objects.Pools.BulletPool), weapon @ r8 (VampireSurvivors.Objects.Weapons.Weapon), index @ r9 (System.Int32)");
			Console.WriteLine("Unknown call target operand: 96 Call \"il2cpp_codegen_initialize_runtime_metadata\", v95 @ rax_v15, typeof(UnityEngine.Vector3), 0, 0, index @ r9 (System.Int32)");
			float num5 = num | 0;
			_ = 1L;
			num4 = num5;
		}
		nint num6 = (nint)new Vector3();
		object obj8 = new string("Unmanaged memory load: [v100 @ rcx_v17 (Il2CppClass<UnityEngine.Vector3>)+B8]");
		long num8;
		float num9;
		if ((object)transform != null)
		{
			object obj9 = obj7 | 0L;
			Console.WriteLine("Unknown call target operand: 50 Call \"il2cpp_codegen_initialize_runtime_metadata\", v35 @ rax_v9, typeof(VampireSurvivors.Framework.PhaserTweens.Tweens), pool @ rdx (VampireSurvivors.Objects.Pools.BulletPool), weapon @ r8 (VampireSurvivors.Objects.Weapons.Weapon), index @ r9 (System.Int32)");
			float num7 = obj9 | 0L;
			num8 = 1L;
			num = num7;
			bool flag3 = new string("Unmanaged memory load: [189999AEE]") != null;
			num9 = num4;
			if (flag3)
			{
				goto IL_02b3;
			}
		}
		Console.WriteLine("Method not found @180AB7550");
		Weapon weapon2 = weapon;
		Console.WriteLine("Unknown call target operand: 129 Call \"il2cpp_codegen_initialize_runtime_metadata\", v135 @ rax_v20, methodof(UnityEngine.Object+MarshalledUnityObject::MarshalNotNull), 0, 0, index @ r9 (System.Int32)");
		float num10 = num4 | 0;
		num8 = 1L;
		num9 = num10;
		goto IL_02b3;
		IL_02b3:
		bool flag4 = new string("Unmanaged memory load: [v75 @ rax_v14 (UnityEngine.Transform)+10]") == null;
		Transform transform2 = transform;
		if (!flag4)
		{
			object obj12 = default(object);
			int num12 = default(int);
			object obj15 = default(object);
			Weapon weapon5 = default(Weapon);
			Transform transform5 = default(Transform);
			Weapon weapon6 = default(Weapon);
			TweenConfig tweenConfig = default(TweenConfig);
			int num14 = default(int);
			int num15 = default(int);
			object obj18 = default(object);
			object obj19 = default(object);
			Weapon weapon7 = default(Weapon);
			Weapon weapon8 = default(Weapon);
			Transform transform7 = default(Transform);
			Transform transform8 = default(Transform);
			object obj21 = default(object);
			bool flag17 = default(bool);
			object obj28 = default(object);
			WhipProjectile whipProjectile2 = default(WhipProjectile);
			object obj33 = default(object);
			object obj34 = default(object);
			TweenConfig tweenConfig2 = default(TweenConfig);
			int num18 = default(int);
			bool flag21 = default(bool);
			object obj38 = default(object);
			object obj39 = default(object);
			Transform transform10 = default(Transform);
			float num20 = default(float);
			Transform transform11 = default(Transform);
			object obj43 = default(object);
			bool flag31 = default(bool);
			SoundManager.SoundConfig soundConfig = default(SoundManager.SoundConfig);
			object obj53 = default(object);
			object obj54 = default(object);
			Transform transform12 = default(Transform);
			object obj60 = default(object);
			object obj62 = default(object);
			Weapon weapon11 = default(Weapon);
			Transform transform13 = default(Transform);
			WhipProjectile whipProjectile4 = default(WhipProjectile);
			object obj65 = default(object);
			bool flag57 = default(bool);
			object obj67 = default(object);
			object obj73 = default(object);
			object obj74 = default(object);
			while (true)
			{
				Console.WriteLine("Method not found @180AB7B30");
				Console.WriteLine("");
				object obj10 = 0;
				object obj11 = obj12;
				Weapon weapon3 = weapon2;
				Transform transform3 = transform;
				if (new string("Unmanaged memory load: [189999BA0]") == null)
				{
					break;
				}
				bool flag61;
				do
				{
					Console.WriteLine("Indirect call: 173 IndirectCall v291 @ rax_v24 (should have been resolved before IL gen)");
					bool flag5 = _scaleTween == null;
					object obj13 = obj8;
					if (!flag5)
					{
						goto IL_03da;
					}
					goto IL_0445;
					IL_0445:
					Console.WriteLine("Unknown call target operand: 191 Call \"il2cpp_vm_object_new\", v324 @ rax_v25 (VampireSurvivors.Framework.PhaserTweens.TweenConfig), typeof(VampireSurvivors.Framework.PhaserTweens.TweenConfig), v320 @ rdx_v6, 0, index @ r9 (System.Int32)");
					Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.PhaserTweens.TweenConfig::.ctor");
					((TweenConfig)0)..ctor();
					object obj61;
					object obj63;
					bool flag42;
					while (true)
					{
						IL_046b:
						Console.WriteLine("Method not found @180AB7B30");
						Console.WriteLine("䜀攀渀攀爀椀挀⸀䤀䔀渀甀洀攀爀愀琀漀爀怀\u3100㰀嘀");
						int num11 = num12;
						bool flag6 = false;
						object obj14 = obj15;
						Weapon weapon4 = weapon5;
						Transform transform4 = transform5;
						Console.WriteLine("Ⰰ\u2000琀礀瀀攀漀昀⠀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀");
						bool flag7 = (object)weapon6 == null;
						int num13 = index;
						bool flag8 = true;
						object obj16 = 0;
						Console.WriteLine("Constructor not found for: Type: System.Object[1] (probably static type)");
						object obj17 = tweenConfig;
						Transform transform6 = (Transform)(object)tweenConfig;
						if (!flag7)
						{
							goto IL_04ff;
						}
						goto IL_0552;
						IL_0552:
						Console.WriteLine("Method not found @180AB7550");
						transform2 = transform6;
						goto IL_0565;
						IL_0565:
						UnityEngine.Bindings.ThrowHelper.ThrowNullReferenceException(0);
						num14 = num15;
						bool flag9 = false;
						obj18 = obj19;
						object obj20 = weapon7;
						weapon8 = weapon7;
						transform7 = transform8;
						ProfilerMarker markerCleanup = Weapon._markerCleanup;
						Console.WriteLine("Method not found @180B02C70");
						bool flag10 = obj21 == null;
						obj10 = new string("攀洀⸀䤀䐀椀猀瀀漀猀愀戀氀攀⤀Ⰰ\u2000洀攀琀栀漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀Ⰰ\u2000瘀㈀㠀\u2000䀀\u2000爀㠀Ⰰ\u2000瘀㈀㤀\u2000䀀\u2000爀㤀\u2000⠀䤀氀㈀");
						obj11 = new string("");
						weapon3 = weapon6;
						transform3 = (Transform)(object)tweenConfig;
						if (flag10)
						{
							break;
						}
						goto IL_05fd;
						IL_05fd:
						Console.WriteLine("Method not found @1800022B0");
						bool flag11 = tweenConfig == null;
						num13 = index;
						flag8 = false;
						obj16 = new string("");
						obj17 = weapon6;
						transform6 = (Transform)(object)tweenConfig;
						if (!flag11)
						{
							while (true)
							{
								Console.WriteLine("Method not found @180AB7550");
								int num16 = num14;
								object obj22 = obj18;
								Weapon weapon9 = weapon8;
								Transform transform9 = transform7;
								bool flag12 = new string("Unmanaged memory load: [18996C9E0]") == null;
								object obj23 = new string("Unmanaged memory load: [1899A9880]");
								bool flag13 = false;
								object obj24 = new string("");
								if (!flag12)
								{
									goto IL_06c0;
								}
								goto IL_0746;
								IL_0746:
								object obj25 = new string("");
								bool flag14 = new string("") == null;
								num13 = index;
								flag8 = flag13;
								obj16 = obj24;
								obj17 = new string("");
								transform6 = (Transform)(object)tweenConfig;
								if (flag14)
								{
									break;
								}
								while (true)
								{
									Console.WriteLine("Not implemented instruction: bts rcx,r8");
									bool flag15 = (object)new string("") != new string("");
									object obj26 = obj25;
									object obj27 = new string("洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䤀䔀渀甀洀攀爀愀戀氀攀怀\u3100㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀");
									Console.WriteLine("甀爀瘀椀瘀漀爀猀⸀倀氀愀礀攀爀䤀渀昀漀㸀⤀Ⰰ\u2000洀攀琀栀漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀Ⰰ\u2000");
									_ = 1120403456L;
									_ = 1L;
									_ = 1L;
									nint num17 = (nint)new Tweens();
									if (new string("") != null)
									{
										break;
									}
									new string("");
									bool flag16 = (object)new string("") != new string("");
									flag13 = flag17;
									obj24 = obj28;
									if (!flag16)
									{
										Console.WriteLine("");
										break;
									}
								}
								MultiTargetTween scaleTween = Tweens.Add((TweenConfig)0);
								bool flag18 = new string("Unmanaged memory load: [18996C9E0]") == null;
								whipProjectile2._scaleTween = scaleTween;
								object obj29 = 0;
								object obj30 = obj24;
								if (!flag18)
								{
									object obj31 = (object)_scaleTween >> 12;
									object obj32 = obj31 & 0x1FFFFFL;
									obj33 = obj32 >> 6;
									obj34 = obj32 & 0x3FL;
								}
								object obj35;
								object obj36;
								object obj37;
								if (_alphaTween == null)
								{
									Console.WriteLine("");
									obj35 = tweenConfig2;
									Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.PhaserTweens.TweenConfig::.ctor");
									obj36 = tweenConfig2;
									((TweenConfig)0)..ctor();
									obj37 = new string("");
									bool flag19 = (object)new string("") != new string("");
									obj29 = obj33;
									obj30 = obj34;
									if (!flag19)
									{
										goto IL_0a19;
									}
								}
								Console.WriteLine("Not implemented instruction: bts rcx,r8");
								bool flag20 = (object)new string("") != new string("");
								obj36 = _alphaTween;
								Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.PhaserTweens.MultiTargetTween::Restart");
								obj37 = _alphaTween;
								((MultiTargetTween)0).Restart();
								num18 = index;
								flag21 = false;
								obj38 = obj30;
								obj39 = _alphaTween;
								transform10 = (Transform)(object)tweenConfig;
								obj35 = flag20;
								goto IL_0a19;
								IL_10c9:
								Console.WriteLine("");
								bool flag22 = (object)transform10 == null;
								object obj40 = (object)_alphaTween >> 12;
								object obj41 = obj40 & 0x1FFFFFL;
								bool flag23 = (byte)(obj41 >> 6) != 0;
								object obj42 = obj41 & 0x3FL;
								Weapon weapon10 = (Weapon)(object)new string("");
								bool flag24 = new string("") == null;
								num14 = num18;
								flag9 = false;
								obj18 = obj38;
								obj20 = new string("");
								weapon8 = (Weapon)(object)new string("");
								transform7 = transform10;
								if (flag24)
								{
									continue;
								}
								float num19;
								while (true)
								{
									Console.WriteLine("Not implemented instruction: bts rcx,r8");
									bool flag25 = (object)new string("琀⤀Ⰰ\u2000嬀瘀㐀㤀㔀\u2000䀀\u2000爀搀砀开瘀\u3100㔀⬀㠀崀Ⰰ\u2000瘀㈀㠀㐀\u2000䀀\u2000爀㠀开瘀㠀Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀琀") != new string("琀⤀Ⰰ\u2000嬀瘀㐀㤀㔀\u2000䀀\u2000爀搀砀开瘀\u3100㔀⬀㠀崀Ⰰ\u2000瘀㈀㠀㐀\u2000䀀\u2000爀㠀开瘀㠀Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀琀");
									bool flag26 = new string("Unmanaged memory load: [189999AE1]") != null;
									num19 = num20;
									if (flag26)
									{
										break;
									}
									new string("琀⤀Ⰰ\u2000嬀瘀㐀㤀㔀\u2000䀀\u2000爀搀砀开瘀\u3100㔀⬀㠀崀Ⰰ\u2000瘀㈀㠀㐀\u2000䀀\u2000爀㠀开瘀㠀Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀琀");
									bool flag27 = (object)new string("琀⤀Ⰰ\u2000嬀瘀㐀㤀㔀\u2000䀀\u2000爀搀砀开瘀\u3100㔀⬀㠀崀Ⰰ\u2000瘀㈀㠀㐀\u2000䀀\u2000爀㠀开瘀㠀Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀琀") != new string("琀⤀Ⰰ\u2000嬀瘀㐀㤀㔀\u2000䀀\u2000爀搀砀开瘀\u3100㔀⬀㠀崀Ⰰ\u2000瘀㈀㠀㐀\u2000䀀\u2000爀㠀开瘀㠀Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀琀");
									num18 = 0;
									flag21 = flag23;
									obj38 = obj42;
									obj39 = new string("琀⤀Ⰰ\u2000嬀瘀㐀㤀㔀\u2000䀀\u2000爀搀砀开瘀\u3100㔀⬀㠀崀Ⰰ\u2000瘀㈀㠀㐀\u2000䀀\u2000爀㠀开瘀㠀Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀琀");
									transform10 = transform11;
									if (!flag27)
									{
										Console.WriteLine("");
										float num21 = num20 | 0;
										_ = 1L;
										num19 = num21;
										break;
									}
								}
								bool flag28 = new string("䔀渀最椀渀攀⸀伀戀樀攀挀琀⤀Ⰰ\u2000嬀瘀㐀㤀㔀\u2000䀀\u2000爀搀砀开瘀\u3100㔀⬀㠀崀Ⰰ\u2000瘀㈀㠀㐀\u2000䀀\u2000爀㠀开瘀㠀Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀") == null;
								num15 = num18;
								obj19 = obj38;
								weapon7 = (Weapon)(object)new string("");
								transform8 = transform10;
								if (!flag28)
								{
									if (new string("Unmanaged memory load: [189999B38]") == null)
									{
										Console.WriteLine("");
										bool flag29 = obj43 == null;
										num11 = num18;
										flag6 = false;
										obj14 = obj38;
										weapon4 = (Weapon)(object)new string("");
										transform4 = transform10;
										if (flag29)
										{
											goto IL_04ff;
										}
									}
									Console.WriteLine("");
									bool flag30 = new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀") == null;
									num14 = num18;
									flag9 = flag31;
									obj18 = obj38;
									obj20 = new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
									weapon8 = (Weapon)(object)new string("");
									transform7 = transform10;
									if (flag30)
									{
										continue;
									}
									if ((nint)new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀") < 0)
									{
									}
									Console.WriteLine("Not implemented instruction: sete dl");
									new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
									Console.WriteLine("Unknown operand: Method: UnityEngine.SpriteRenderer::set_flipY");
									((SpriteRenderer)(object)new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀")).flipY = flag31;
									bool flag32 = new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀") == null;
									num14 = num18;
									flag9 = flag31;
									obj18 = 0;
									obj20 = new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
									weapon8 = (Weapon)(object)new string("");
									transform7 = transform10;
									if (flag32)
									{
										continue;
									}
									new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
									Console.WriteLine("Unknown operand: Method: UnityEngine.SpriteRenderer::set_flipX");
									((SpriteRenderer)(object)new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀")).flipX = (byte)(int)transform10 != 0;
									object obj44 = new string("");
									bool flag33 = new string("") == null;
									num14 = num18;
									flag9 = (byte)(int)transform10 != 0;
									obj18 = 0;
									obj20 = new string("");
									weapon8 = (Weapon)(object)new string("");
									transform7 = (Transform)(object)new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
									if (flag33)
									{
										continue;
									}
									bool flag34 = new string("渀猀⸀䜀攀渀攀爀椀挀⸀䤀䔀渀甀洀攀爀愀琀漀爀怀\u3100㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀倀氀愀礀攀爀䤀渀昀漀㸀⤀\0") == null;
									num14 = num18;
									flag9 = (byte)(int)transform10 != 0;
									obj18 = 0;
									obj20 = new string("渀猀⸀䜀攀渀攀爀椀挀⸀䤀䔀渀甀洀攀爀愀琀漀爀怀\u3100㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀倀氀愀礀攀爀䤀渀昀漀㸀⤀\0");
									weapon8 = (Weapon)(object)new string("");
									transform7 = (Transform)(object)new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
									if (flag34)
									{
										continue;
									}
									new string("渀猀⸀䜀攀渀攀爀椀挀⸀䤀䔀渀甀洀攀爀愀琀漀爀怀\u3100㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀倀氀愀礀攀爀䤀渀昀漀㸀⤀\0");
									new string("渀猀⸀䜀攀渀攀爀椀挀⸀䤀䔀渀甀洀攀爀愀琀漀爀怀\u3100㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀倀氀愀礀攀爀䤀渀昀漀㸀⤀\0");
									int num22 = ((VampireSurvivors.Objects.Characters.CharacterController)0).Depth;
									nint num23 = (nint)new GM();
									bool flag35 = !((bool*)(&this))->m_value;
									num14 = num18;
									flag9 = (byte)(int)new string("愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀㐀㤀㔀\u2000䀀\u2000爀搀砀开瘀\u3100㔀⬀㠀崀\0馀Unmanaged memory ") != 0;
									obj18 = 0;
									obj20 = new GM();
									weapon8 = (Weapon)(object)new string("");
									transform7 = (Transform)(object)new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
									if (flag35)
									{
										continue;
									}
									bool flag36 = new string("Unmanaged memory load: [18996AD90]") != null;
									float num24 = num19;
									if (!flag36)
									{
										Console.WriteLine("唀渀椀琀礀䔀渀最椀渀攀⸀伀戀樀攀挀琀㸀⤀⬀䔀㐀崀\0쮁Unkn");
										float num25 = num19 | 0;
										_ = 1L;
										num24 = num25;
									}
									nint num26 = (nint)new ArcadePhysics();
									object obj45 = new string("怀\u3100㰀嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀倀氀愀礀攀爀䤀渀昀漀㸀⤀\0啕渀洀愀渀愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀");
									object obj46 = new string("");
									bool flag37 = new string("") == null;
									num14 = num18;
									flag9 = (byte)(int)new string("愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀㐀㤀㔀\u2000䀀\u2000爀搀砀开瘀\u3100㔀⬀㠀崀\0馀Unmanaged memory ") != 0;
									obj18 = 0;
									obj20 = obj45;
									weapon8 = (Weapon)(object)new string("");
									transform7 = (Transform)(object)new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
									if (flag37)
									{
										continue;
									}
									object obj47 = new string("攀眀∀Ⰰ\u2000瘀㔀㠀㈀\u2000䀀\u2000爀愀砀开瘀㌀㜀\u2000⠀匀礀猀琀攀洀⸀䐀攀氀攀最愀琀攀⤀Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀琀攀洀⸀䄀挀");
									bool flag38 = new string("攀眀∀Ⰰ\u2000瘀㔀㠀㈀\u2000䀀\u2000爀愀砀开瘀㌀㜀\u2000⠀匀礀猀琀攀洀⸀䐀攀氀攀最愀琀攀⤀Ⰰ\u2000琀礀瀀攀漀昀⠀匀礀猀琀攀洀⸀䄀挀") == null;
									num14 = num18;
									flag9 = (byte)(int)new string("愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀㐀㤀㔀\u2000䀀\u2000爀搀砀开瘀\u3100㔀⬀㠀崀\0馀Unmanaged memory ") != 0;
									obj18 = 0;
									obj20 = obj45;
									weapon8 = (Weapon)(object)new string("");
									transform7 = (Transform)(object)new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
									if (flag38)
									{
										continue;
									}
									nint num27 = (nint)new RenderingExtensions();
									if (new string("⠀匀礀猀琀攀洀⸀䄀挀琀椀漀渀怀㌀㰀匀礀猀琀攀洀⸀䤀渀琀㌀㈀Ⰰ\u2000匀礀猀琀攀洀⸀䤀渀琀㌀㈀Ⰰ\u2000嘀愀洀瀀椀爀") == null)
									{
										Console.WriteLine("");
									}
									bool flag39 = new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀") == null;
									num14 = num18;
									flag9 = (byte)(int)new string("愀最攀搀\u2000洀攀洀漀爀礀\u2000氀漀愀搀㨀\u2000嬀瘀㐀㤀㔀\u2000䀀\u2000爀搀砀开瘀\u3100㔀⬀㠀崀\0馀Unmanaged memory ") != 0;
									obj18 = 0;
									obj20 = new RenderingExtensions();
									weapon8 = (Weapon)(object)new string("");
									transform7 = (Transform)(object)new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
									if (!flag39)
									{
										object obj48 = (object)new string("") >> 31;
										bool flag40 = (nint)obj48 == 1;
										object obj49 = ~(flag40 ? 1 : 0);
										object obj50 = ((obj49 != null) ? ((object)0) : ((object)(-1)));
										object obj51 = (object)new string("") - obj50;
										object obj52 = obj51 >> 1;
										new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
										Console.WriteLine("Unknown operand: Method: UnityEngine.Renderer::set_sortingOrder");
										((Renderer)(object)new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀")).sortingOrder = (int)new string("");
										Console.WriteLine("礀猀琀攀洀⸀匀琀爀椀渀最⤀Ⰰ\u2000瘀㈀㈀\u2000䀀\u2000爀㤀\u2000⠀匀礀猀琀攀洀⸀䈀漀漀氀攀愀渀⤀\0");
										soundConfig.Rate = 1.0653532E+09f;
										soundConfig.Volume = (float?)(object)1L;
										soundConfig.Rate = 1.0737418E+09f;
										float detune = (object)new string("") * (object)new string("Unmanaged memory load: [188A11720]");
										soundConfig.Detune = detune;
										nint num28 = (nint)new SoundManager();
										if (new string("攀琀栀漀搀漀昀⠀匀礀猀琀攀洀⸀一甀氀氀愀戀氀攀怀\u3100㰀匀礀猀琀攀洀⸀匀椀渀最氀攀㸀㨀㨀最攀琀开䠀愀猀嘀愀氀甀攀") == null)
										{
											Console.WriteLine("");
										}
										_ = 13L;
										PlaySoundResult playSoundResult = SoundManager.PlaySound((SfxType)soundConfig, (SoundManager.SoundConfig)0, 10f, (int)num24, (float)obj53);
										return;
									}
									continue;
								}
								goto IL_0565;
								IL_1079:
								Console.WriteLine("");
								bool flag41 = obj54 == null;
								num12 = num18;
								flag42 = flag21;
								obj15 = obj38;
								weapon5 = (Weapon)(object)new string("");
								transform5 = transform10;
								if (flag41)
								{
									goto IL_046b;
								}
								goto IL_10c9;
								IL_06c0:
								UnityEngine.Bindings.ThrowHelper.ThrowNullReferenceException(0);
								num13 = num16;
								flag8 = false;
								obj16 = obj22;
								obj17 = weapon9;
								transform6 = transform9;
								object obj55 = (object)new string("Unmanaged memory load: [v324 @ rax_v25 (VampireSurvivors.Framework.PhaserTweens.TweenConfig)+10]") >> 12;
								object obj56 = obj55 & 0x1FFFFFL;
								flag17 = (byte)(obj56 >> 6) != 0;
								obj28 = obj56 & 0x3FL;
								goto IL_0746;
								IL_0a19:
								while (true)
								{
									object obj57 = new string("");
									bool flag43 = new string("") == null;
									num13 = num18;
									flag8 = flag21;
									obj16 = obj38;
									obj17 = obj39;
									transform6 = transform10;
									if (flag43)
									{
										break;
									}
									Console.WriteLine("Unknown call target operand: 400 Call \"il2cpp_vm_array_new_specific\", v530 @ rax_v46 (UnityEngine.Transform), typeof(System.Object[1]), 1, v520 @ r8_v14, index @ r9 (System.Int32)");
									bool flag44 = (object)transform12 == null;
									num13 = index;
									flag8 = true;
									obj16 = obj30;
									Console.WriteLine("Constructor not found for: Type: System.Object[1] (probably static type)");
									obj17 = obj37;
									transform6 = transform12;
									if (flag44)
									{
										break;
									}
									object obj58 = new string("Unmanaged memory load: [v533 @ rax_v71+58]");
									bool flag45 = new string("Unmanaged memory load: [v533 @ rax_v71+58]") == null;
									num13 = num18;
									flag8 = flag21;
									obj16 = obj38;
									obj17 = new string("Unmanaged memory load: [v533 @ rax_v71+58]");
									transform6 = transform10;
									if (flag45)
									{
										break;
									}
									if (new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀") != null)
									{
										if ((nint)new string("Unmanaged memory load: [v533 @ rax_v71+58]") >= 0)
										{
											goto IL_0c0a;
										}
										object obj59 = transform12;
										Console.WriteLine("Method not found @180B02C70");
										bool flag46 = obj60 == null;
										obj61 = new string("");
										obj62 = obj30;
										obj63 = new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
										weapon11 = (Weapon)(object)tweenConfig2;
										transform13 = transform12;
										if (flag46)
										{
											goto IL_03da;
										}
									}
									Console.WriteLine("Method not found @1800022B0");
									bool flag47 = tweenConfig2 == null;
									num13 = index;
									flag8 = false;
									obj16 = new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
									obj17 = transform12;
									transform6 = transform12;
									if (flag47)
									{
										break;
									}
									goto IL_0c0a;
									IL_0f4f:
									MultiTargetTween multiTargetTween = Tweens.Add((TweenConfig)0);
									bool flag48 = multiTargetTween == null;
									num13 = 0;
									flag8 = false;
									object obj64;
									obj16 = obj64;
									obj17 = tweenConfig2;
									transform6 = transform11;
									if (flag48)
									{
										break;
									}
									if (new string("Unmanaged memory load: [189999B30]") == null)
									{
										new string("瘀㐀㌀\u3100\u2000䀀\u2000爀㠀开瘀㔀⬀㠀⬀瘀㐀㌀㐀\u2000䀀\u2000爀愀砀开瘀㈀㈀崀\0떀Indirect call");
										bool flag49 = (object)new string("瘀㐀㌀\u3100\u2000䀀\u2000爀㠀开瘀㔀⬀㠀⬀瘀㐀㌀㐀\u2000䀀\u2000爀愀砀开瘀㈀㈀崀\0떀Indirect call") != new string("瘀㐀㌀\u3100\u2000䀀\u2000爀㠀开瘀㔀⬀㠀⬀瘀㐀㌀㐀\u2000䀀\u2000爀愀砀开瘀㈀㈀崀\0떀Indirect call");
										WhipProjectile whipProjectile3 = whipProjectile4;
										obj64 = obj65;
										if (flag49)
										{
											goto IL_0f19;
										}
										MultiTargetTween alphaTween = multiTargetTween.SetAutoKill(autoKill: false);
										bool flag50 = new string("Unmanaged memory load: [18996C9E0]") == null;
										whipProjectile2._alphaTween = alphaTween;
										num18 = 0;
										flag21 = false;
										obj38 = 0;
										obj39 = multiTargetTween;
										transform10 = transform11;
										if (flag50)
										{
											continue;
										}
										goto IL_1079;
									}
									goto IL_10c9;
									IL_0d08:
									Weapon weapon12 = (Weapon)(object)new string("");
									bool flag51 = new string("") == null;
									num13 = num18;
									flag8 = flag21;
									obj16 = obj38;
									obj17 = new string("Unmanaged memory load: [v533 @ rax_v71+58]");
									transform6 = transform10;
									if (flag51)
									{
										break;
									}
									while (true)
									{
										Console.WriteLine("Not implemented instruction: bts rcx,r8");
										bool flag52 = (object)new string("") != new string("");
										bool flag53 = new string("Unmanaged memory load: [18996C9E0]") == null;
										WhipProjectile whipProjectile3 = whipProjectile;
										obj64 = new string("洀攀琀栀漀搀䤀渀昀漀\u2000䀀\u2000爀搀砀");
										if (!flag53)
										{
											bool flag54 = new string("Unmanaged memory load: [189999AE0]") != null;
											num20 = num9;
											if (flag54)
											{
												break;
											}
											new string("");
											bool flag55 = (object)new string("") != new string("");
											bool flag56 = flag57;
											object obj66 = obj67;
											if (flag55)
											{
												continue;
											}
											object obj68 = (object)new string("䌀氀愀猀猀㰀匀礀猀琀攀洀⸀䌀漀氀氀攀挀琀椀漀渀猀⸀䜀攀渀攀爀椀挀⸀䤀䔀渀甀洀攀爀愀琀漀爀怀\u3100㰀嘀愀洀瀀椀爀攀匀甀") >> 12;
											object obj69 = obj68 & 0x1FFFFFL;
											whipProjectile4 = (WhipProjectile)(obj69 >> 6);
											obj65 = obj69 & 0x3FL;
										}
										nint num29 = (nint)new Tweens();
										if (new string("\u2000漀瀀攀爀愀渀搀㨀\u2000\u3100\u3000㌀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开挀漀搀攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀") == null)
										{
											Console.WriteLine("");
											float num30 = num9 | 0;
											_ = 1L;
											num20 = num30;
											break;
										}
										goto IL_0f4f;
									}
									bool flag58 = new string("") == null;
									num16 = num18;
									obj22 = obj38;
									weapon9 = (Weapon)(object)new string("");
									transform9 = transform10;
									if (flag58)
									{
										goto IL_06c0;
									}
									goto IL_0f19;
									IL_0c0a:
									if (new string("") != null)
									{
										bool flag59 = new string("Unmanaged memory load: [18996C9E0]") == null;
										bool flag56 = false;
										object obj66 = new string("攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀攀琀愀搀愀琀愀∀Ⰰ\u2000瘀㈀㘀\u2000䀀\u2000爀愀砀开瘀\u3100Ⰰ\u2000琀");
										if (!flag59)
										{
											Console.WriteLine("Not implemented instruction: setne bl");
											goto IL_0d08;
										}
									}
									else
									{
										Console.WriteLine("Not implemented instruction: sete bl");
										object obj70 = (object)new string("爀愀渀搀㨀\u2000㐀㈀\u2000䌀愀氀氀\u2000∀椀氀㈀挀瀀瀀开挀漀搀攀最攀渀开椀渀椀琀椀愀氀椀稀攀开爀甀渀琀椀洀攀开洀") >> 12;
										object obj71 = obj70 & 0x1FFFFFL;
										flag57 = (byte)(obj71 >> 6) != 0;
										obj67 = obj71 & 0x3FL;
									}
									_ = 1120403456L;
									_ = 1L;
									_ = 1120403456L;
									_ = 1L;
									Console.WriteLine("");
									object obj72 = whipProjectile;
									Console.WriteLine("Method not found @182C49640");
									goto IL_0d08;
									IL_0f19:
									Console.WriteLine("Not implemented instruction: bts rcx,r8");
									bool flag60 = (object)new string("瘀㐀㌀\u3100\u2000䀀\u2000爀㠀开瘀㔀⬀㠀⬀瘀㐀㌀㐀\u2000䀀\u2000爀愀砀开瘀㈀㈀崀\0떀Indirect call") != new string("瘀㐀㌀\u3100\u2000䀀\u2000爀㠀开瘀㔀⬀㠀⬀瘀㐀㌀㐀\u2000䀀\u2000爀愀砀开瘀㈀㈀崀\0떀Indirect call");
									Console.WriteLine("最攀渀⤀\0馀Unmanaged ");
									goto IL_0f4f;
								}
								break;
							}
						}
						goto IL_0552;
						IL_04ff:
						Console.WriteLine("Method not found @180AB7B30");
						Console.WriteLine("㨀\u2000嘀愀洀瀀椀爀攀匀甀爀瘀椀瘀漀爀猀⸀唀䤀⸀匀攀氀攀挀琀愀戀氀攀唀䤀㨀㨀伀渀䐀攀猀琀爀漀礀\0궀Unm");
						num15 = num11;
						obj19 = obj14;
						weapon7 = weapon4;
						transform8 = transform4;
						if (new string("") != null)
						{
							goto IL_0552;
						}
						goto IL_05fd;
					}
					Console.WriteLine("Method not found @180AB7780");
					Console.WriteLine("Method not found @180AB7510");
					obj61 = 0;
					obj62 = 0;
					obj63 = obj73;
					weapon11 = weapon3;
					transform13 = transform3;
					Console.WriteLine("Unknown call target operand: 158 Call \"InternalCalls_Resolve\", v264 @ rax_v23, [187CA1940], 0, 0, index @ r9 (System.Int32)");
					flag61 = obj74 == null;
					weapon2 = (Weapon)(object)new string("Unmanaged memory load: [v75 @ rax_v14 (UnityEngine.Transform)+10]");
					continue;
					IL_03da:
					Console.WriteLine("Method not found @180AB7780");
					Console.WriteLine("Method not found @180AB7510");
					num12 = index;
					flag42 = false;
					obj15 = obj62;
					weapon5 = weapon11;
					transform5 = transform13;
					_ = _scaleTween;
					Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.PhaserTweens.MultiTargetTween::Kill");
					_ = _scaleTween;
					((MultiTargetTween)0).Kill();
					obj13 = 0;
					goto IL_0445;
				}
				while (!flag61);
			}
		}
		UnityEngine.Bindings.ThrowHelper.ThrowNullReferenceException(0);
	}

	public override void Despawn()
	{
		//IL_003a: Expected O, but got I4
		//IL_0090: Expected O, but got I4
		//IL_0074: Expected O, but got I4
		//IL_003a->IL003a: Incompatible stack heights: 2 vs 0
		//IL_0074->IL0074: Incompatible stack heights: 2 vs 0
		if (_scaleTween != null)
		{
			_ = _scaleTween;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.PhaserTweens.MultiTargetTween::Kill");
			_ = _scaleTween;
			((MultiTargetTween)0).Kill();
		}
		if (_alphaTween != null)
		{
			_ = _alphaTween;
			Console.WriteLine("Unknown operand: Method: VampireSurvivors.Framework.PhaserTweens.MultiTargetTween::Pause");
			_ = _alphaTween;
			((MultiTargetTween)0).Pause();
		}
		Console.WriteLine("Unknown operand: Method: VampireSurvivors.Objects.Projectiles.Projectile::Despawn");
		((Projectile)0).Despawn();
	}

	public WhipProjectile()
	{
		//IL_0041: Expected I, but got O
		//IL_008c: Expected O, but got I4
		//IL_0038->IL0038: Incompatible stack heights: 1 vs 0
		if ((long)new string("") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 17 Call \"il2cpp_codegen_initialize_runtime_metadata\", v16 @ rax_v1, typeof(VampireSurvivors.Objects.Projectiles.Projectile), methodInfo @ rdx, v18 @ r8, v19 @ r9");
			_ = 1L;
		}
		nint num = (nint)new Projectile();
		if ((long)new string("Unmanaged memory load: [v26 @ rcx_v3 (Il2CppClass<VampireSurvivors.Objects.Projectiles.Projectile>)+E4]") == 0L)
		{
			Console.WriteLine("Unknown call target operand: 33 Call \"il2cpp_runtime_class_init_actual\", v38 @ rax_v3, typeof(VampireSurvivors.Objects.Projectiles.Projectile), methodInfo @ rdx, v18 @ r8, v19 @ r9");
		}
		Console.WriteLine("Unknown operand: Method: VampireSurvivors.Objects.Projectiles.Projectile::.ctor");
		((Projectile)0)..ctor();
	}
}
