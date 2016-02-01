#ifndef __UVISOR_DEBUG_BOX_HW_H__
#define __UVISOR_DEBUG_BOX_HW_H__



#if defined(TARGET_LIKE_STM32F429I_DISCO)

#define DEBUG_BOX_ACL(acl_list_name)                      \
    static const UvisorBoxAclItem acl_list_name[] = {     \
        {GPIOB,  sizeof(*GPIOB),  UVISOR_TACLDEF_PERIPH}, \
        {RCC,    sizeof(*RCC),    UVISOR_TACLDEF_PERIPH}, \
        {USART3, sizeof(*USART3), UVISOR_TACLDEF_PERIPH}, \
    }

#else

#define DEBUG_BOX_ACL(acl_list_name) \
    static const UvisorBoxAclItem acl_list_name[] = {}
    
#endif




#endif/*__UVISOR_DEBUG_BOX_CONTEXT_H__*/
